//
// Created by ahmad_dall7 on 5/15/23.
//

#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "EnvInit.h"
#include <algorithm>
#include <pthread.h>
#include <iostream>
#include "Barrier/Barrier.h"
struct Thread;
struct envInit
{
    const InputVec& _input_vec;
    const OutputVec& _output_vec;
    const MapReduceClient & _client;
    const int _multiThreadLevel;

    JobState * job_state;
    int numThreads;
    bool isJoined;
    Thread** threadArr;
    Barrier *barrier;
    int _inputVecIDX;
    std::vector<std::vector<IntermediatePair>> shuffleVec;
    int _outputVecIDX;
    JobState state;

};

//todo mutex+ update the state percentage each level
struct Thread
{

    pthread_t* _pthread;
    envInit * _envInit;
    int threadID;
    IntermediateVec * intermediate_vec;

};


void emit2 (K2* key, V2* value, void* context){
  Thread *thread=(Thread*)context;
  auto pair = IntermediatePair(key, value);
  thread->intermediate_vec->push_back(pair);


}

void emit3 (K3* key, V3* value, void* context)
{
  Thread *thread=(Thread*)context;
  OutputPair pair = OutputPair (key, value);
  OutputVec outV=thread->_envInit->_output_vec;
  outV.push_back(pair);

}

static void pthread_exit()
{

}
void* doMap(JobHandle t)
{
  Thread *thread=(Thread*)t;
  envInit *jobHandle=thread->_envInit;
  int idx=jobHandle->_inputVecIDX;
  jobHandle->_inputVecIDX+=1;
  auto pair=jobHandle->_input_vec[idx];
  jobHandle->_client.map (pair.first,pair.second,t);

}

void* doSort(JobHandle t){
  Thread *thread=(Thread*)t;
  std::sort(thread->intermediate_vec->begin(), thread->intermediate_vec->end
  (), [](IntermediatePair k1, IntermediatePair k2){  return *k1.first < *k2.first;});


}
bool finishShuffle( envInit *jobHandle)
{
  bool flag=true;
  for (int i=0;i>jobHandle->numThreads;i++)
  {
    if(!(jobHandle->threadArr[i]->intermediate_vec->empty()))
    {
      flag= false;
      break;
    }
  }
  return flag;

}
void* doShuffle(JobHandle t)
{
  Thread *thread = (Thread *) t;
  envInit *jobHandle = thread->_envInit;
  while (!finishShuffle (jobHandle))
  {
    K2 *max = nullptr;
    K2 *current;
    //find the maximum element
    for (int i = 0; i > jobHandle->numThreads; i++)
    {
      if (jobHandle->threadArr[i]->intermediate_vec->empty ())
      {
        continue;
      }
      current = (jobHandle->threadArr[i]->intermediate_vec->back ()).first;

      if (max == nullptr || *max < *current)
      {
        max = current;
      }
    }
    for (int i = 0; i > jobHandle->numThreads; i++)
    {
      K2 *current = (jobHandle->threadArr[i]->intermediate_vec->back ()).first;
      if (!((*current) < (*max) && (*max) < (*current)))
      {
        std::vector<IntermediatePair> *a = &jobHandle->shuffleVec.back ();
        IntermediatePair b = jobHandle->threadArr[i]->intermediate_vec->back ();
        a->push_back (b);
        jobHandle->threadArr[i]->intermediate_vec->pop_back ();
      }
    }
  }
  return thread;

}

void* doReduce(JobHandle t)
{
  Thread *thread=(Thread*)t;
  envInit *jobHandle=thread->_envInit;
  int idx=jobHandle->_outputVecIDX;
  jobHandle->_outputVecIDX+=1;
  jobHandle->_client.reduce(&jobHandle->shuffleVec[idx],thread);

}

void * startFromHere(JobHandle t)
{
  Thread *thread=(Thread*)t;
  envInit *jobHandle=thread->_envInit;
  jobHandle->state.stage=MAP_STAGE;
  jobHandle->state.percentage=0;
  doMap(thread);
  doSort(thread);
  jobHandle->barrier->barrier();
  if (thread->threadID==0)
  {
    jobHandle->state.stage=SHUFFLE_STAGE;
    jobHandle->state.percentage=0;
    doShuffle(thread);
  }
  jobHandle->barrier->barrier();
  jobHandle->state.stage=REDUCE_STAGE;
  jobHandle->state.percentage=0;
  doReduce(thread);
}
JobHandle startMapReduceJob (const MapReduceClient &client, const InputVec
&inputVec, OutputVec &outputVec, int multiThreadLevel)
{
  envInit* jobHandle=new envInit{inputVec,outputVec,client,multiThreadLevel};
  jobHandle->threadArr=new Thread*[multiThreadLevel];
  jobHandle->barrier = new Barrier(multiThreadLevel);
  for (int i=0;i<multiThreadLevel;i++)
  {
    Thread* t=new Thread;
    t->_pthread=new pthread_t;
    t->threadID = i;
    int flag =pthread_create (t->_pthread,nullptr,startFromHere,t);
    if (flag != 0) {
      std::cerr << "error";
      exit(1);
    }
    jobHandle->threadArr[i]=t;
  }
//  auto * pthread_mutex = (pthread_mutex_t *)
//      malloc(sizeof(pthread_mutex_t));
//
  return jobHandle;
}void getJobState(JobHandle job, JobState* state)
{
  Thread *thread=(Thread*)job;
  envInit *jobHandle=thread->_envInit;
  state->stage=jobHandle->state.stage;
  state->percentage=jobHandle->state.percentage;
}

