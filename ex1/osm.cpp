//
// Created by ahdal_9lda2pd on 04/04/2023.
//

#include "osm.h"

#include <sys/time.h>
#include <cmath>
#include <iostream>


void empty_function()
{

}



double osm_operation_time (unsigned int iterations)
{
  if (iterations <= 0)
  {
    return -1;
  }

  timeval start, end;

  gettimeofday (&start, NULL);


  int x = 0;
  for (int i=0 ; i<iterations ; i += 10)
  {
    x+=1;
    x+=1;
    x+=1;
    x+=1;
    x+=1;
    x+=1;
    x+=1;
  }

  gettimeofday (&end, nullptr);


  double dSeconds = end.tv_sec - start.tv_sec;
  double dMSeconds = end.tv_usec - start.tv_usec;

  double totalM = (dSeconds * 1000000.0) + dMSeconds;

  double totalNano = totalM * 1000.0;


  return totalNano / iterations;
}


double osm_function_time (unsigned int iterations)
{
  if (iterations <= 0)
  {
    return -1;
  }

  timeval start, end;

  gettimeofday (&start, NULL);

  int i=0;

  for (int i=0 ; i<iterations ; i += 10)
  {
    empty_function();
    empty_function();
    empty_function();
    empty_function();
    empty_function();
    empty_function();
    empty_function();
  }

  gettimeofday (&end, nullptr);


  double dSeconds = end.tv_sec - start.tv_sec;
  double dMSeconds = end.tv_usec - start.tv_usec;

  double totalM = (dSeconds * 1000000.0) + dMSeconds;

  double totalNano = totalM * 1000.0;


  return totalNano / iterations;
}


double osm_syscall_time (unsigned int iterations)
{
  if (iterations <= 0)
  {
    return -1;
  }

  timeval start, end;

  gettimeofday (&start, NULL);

  int i=0;

  for (int i=0 ; i<iterations ; i += 10)
  {
    OSM_NULLSYSCALL; //1
    OSM_NULLSYSCALL; //2
    OSM_NULLSYSCALL; //3
    OSM_NULLSYSCALL; //4
    OSM_NULLSYSCALL; //5
    OSM_NULLSYSCALL; //6
    OSM_NULLSYSCALL; //7
  }

  gettimeofday (&end, nullptr);


  double dSeconds = end.tv_sec - start.tv_sec;
  double dMSeconds = end.tv_usec - start.tv_usec;

  double totalM = (dSeconds * 1000000.0) + dMSeconds;

  double totalNano = totalM * 1000.0;


  return totalNano / iterations;

}


int main()
{
  std::cout << "operating : " << osm_operation_time(1000) << std::endl;
  std::cout << "empty func : " << osm_function_time(1000) << std::endl;
  std::cout << "empty trap func : " << osm_syscall_time(1000) << std::endl;
}

