#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define PATH_WEIGHT 1
#define ROOT_FRAME 0
#define RESET_VALUE 0

typedef struct {
    uint64_t frameIndex;
    uint64_t evictedPageIndex;
    uint64_t physicalAddress;
} evect_details_t;


static uint64_t GetPageNumber(uint64_t virtualAddress, int translation)
{
  static const int mask = PAGE_SIZE - 1;
  return (virtualAddress >> (translation * OFFSET_WIDTH)) & mask;
}

static uint64_t GetOffset(uint64_t virtualAddress)
{
  return virtualAddress & (VIRTUAL_ADDRESS_WIDTH - PAGE_SIZE);

}

static void resetFrame(uint64_t frame)
{
  for (std::size_t i = 0; i < PAGE_SIZE; ++i)
  {
    PMwrite(frame * PAGE_SIZE + i, RESET_VALUE);
  }
}

static uint64_t traverse_ex(uint64_t depth, uint64_t baseAddress,
                            uint64_t parentAddress, uint64_t pageIndex,
                            word_t& maxAddress, evect_details_t &evect_details,
                            int pathWeight, int& maxPathWeight, bool& isEmpty)
{
  if (baseAddress > static_cast<uint64_t>(maxAddress))
  {
    maxAddress = static_cast<word_t>(baseAddress);
  }

  if (depth == TABLES_DEPTH) {
    return ROOT_FRAME;
  }

  for (uint64_t entry = 0; entry < PAGE_SIZE; ++entry) {
    word_t frameIndex = RESET_VALUE;
    PMread(entry + baseAddress * PAGE_SIZE, &frameIndex);

    if (frameIndex == RESET_VALUE)
    {
      continue;
    }

    if (frameIndex % 2 == 0) {
      pathWeight += PATH_WEIGHT;
    }

    uint64_t nextPageIndex = (pageIndex << OFFSET_WIDTH) + entry;
    if (depth == TABLES_DEPTH - 1 && pathWeight > maxPathWeight) {
      evect_details.evictedPageIndex = nextPageIndex;
      evect_details.frameIndex = frameIndex;
      evect_details.physicalAddress = baseAddress * PAGE_SIZE + entry;
      maxPathWeight = pathWeight;
    }

    uint64_t frameAddress = traverse_ex(depth + 1, frameIndex, parentAddress,
                                        nextPageIndex, maxAddress,
                                        evect_details, pathWeight,
                                        maxPathWeight, isEmpty);

    if (!isEmpty && frameAddress != ROOT_FRAME)
    {
      return frameAddress;
    }

    isEmpty = false;
  }

  return ROOT_FRAME;
}

static uint64_t traverse(uint64_t parentAddress, word_t& maxAddress,
                         evect_details_t &evect_details)
{
  bool emptyFrame = true;
  int maxWeight = 0;

  return traverse_ex(0, ROOT_FRAME, parentAddress, RESET_VALUE, maxAddress,
                     evect_details, 0, maxWeight, emptyFrame);
}

static uint64_t GetFreeFrame(uint64_t parentAddress)
{
  word_t maxAddress = ROOT_FRAME;
  evect_details_t evect_details = { ROOT_FRAME, ROOT_FRAME, ROOT_FRAME};

  uint64_t frameAddress = traverse(parentAddress, maxAddress, evect_details);

  if (frameAddress != ROOT_FRAME)
  {
    return frameAddress;
  }

  if (maxAddress + 1 < NUM_FRAMES)
  {
    return maxAddress + 1;
  }

  PMevict(evect_details.frameIndex, evect_details.evictedPageIndex);
  PMwrite(evect_details.physicalAddress, RESET_VALUE);
  return evect_details.frameIndex;
}

static word_t SwapFrame(uint64_t virtualAddress, int translation,
                        uint64_t pageNum, uint64_t currentFrame,
                        uint64_t parentAddress)
{
  uint64_t freePage = GetFreeFrame(parentAddress);
  if (translation == 1)
  {
    PMrestore(freePage, virtualAddress >> OFFSET_WIDTH);
  }
  else
  {
    resetFrame(freePage);
  }

  word_t addr = static_cast<word_t>(freePage);
  PMwrite(currentFrame + pageNum, addr);
  return addr;
}

static uint64_t GetPhysicalAddress(uint64_t virtualAddress)
{
  uint64_t parentAddress = ROOT_FRAME;
  word_t addr = RESET_VALUE;

  for (int translation = TABLES_DEPTH; translation > 0; --translation)
  {
    uint64_t pageNum = GetPageNumber(virtualAddress, translation);
    uint64_t currentFrame = addr*PAGE_SIZE;

    PMread(currentFrame + pageNum, &addr);

    if (addr == RESET_VALUE)
    {
      addr = SwapFrame(virtualAddress, translation, pageNum, currentFrame,
                       parentAddress);
    }

    parentAddress = addr;
  }

  return addr * PAGE_SIZE + GetOffset(virtualAddress);
}

void VMinitialize()
{
  resetFrame(ROOT_FRAME);
}

int VMread(uint64_t virtualAddress, word_t* value)
{
  if (virtualAddress >= VIRTUAL_MEMORY_SIZE)
  {
    return 0;
  }

  PMread(GetPhysicalAddress(virtualAddress), value);

  return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value)
{
  if (virtualAddress >= VIRTUAL_MEMORY_SIZE)
  {
    return 0;
  }

  PMwrite(GetPhysicalAddress(virtualAddress), value);

  return 1;
}