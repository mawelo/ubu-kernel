#ifndef FINAL_H
#define FINAL_H

#include <functional>

/*
  example:
  finally close_idxfile([&]
  {
   if( ProgOpts["VERBOSELEVEL"] > 1 ) inf << __FILE__ << " : " << __LINE__ << " : " << __func__ << " : INFO: close index file." << std::endl;
   idxfile.close();
  });
*/
class finally
{
  std::function<void(void)> functor;

  public:
   finally(const std::function<void(void)> &functor) : functor(functor) {}
   ~finally()
   {
    functor();
   }
};

#endif
