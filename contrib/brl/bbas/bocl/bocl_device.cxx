#include "bocl_device.h"
#include "bocl_utils.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h> // for std::malloc() and std::free()
#include <vcl_cstring.h>

bocl_device::bocl_device(cl_device_id& device)
{  
  device_ = new cl_device_id[1]; 
  device_[0] = device; 
  info_ = bocl_device_info(device_); 

  //Create a context from the device ID
  int status = 1;
  context_ = clCreateContext(0, 1, device_, NULL, NULL, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateContextFromType failed: " + error_to_string(status))) {
    return;
  }
}

//destructor
bocl_device::~bocl_device()
{
  if(context_) clReleaseContext(context_);
  if(device_) delete[] device_; 
}


vcl_ostream& operator <<(vcl_ostream &s, bocl_device& dev)
{
  s << dev.info() << vcl_endl;
  return s;
}


