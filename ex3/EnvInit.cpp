//
// Created by ahmad_dall7 on 5/22/23.
//

#include "EnvInit.h"

static void init(const int multiThreadLevel, const MapReduceClient & client)
{

  auto _threads = new pthread_t*[multiThreadLevel];
  for ( int i = 0 ; i < multiThreadLevel; i++ ) {
    _threads[i] = new pthread_t;
  }

}


EnvInit::EnvInit (const MapReduceClient &client, const InputVec &inputVec,
                  OutputVec &outputVec, int multiThreadLevel) : _client
                  (&client),_input_vec(inputVec), _output_vec(outputVec),
                  _multiThreadLevel(multiThreadLevel)
{
  init (multiThreadLevel, client);
}


