#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <sstream>
#include <mutex>

#define log(x) std::cout << x << std::endl;
#define log_err(x) { log("[error]:" << x ); exit(1); }
#define log_sync(mx,z) { std::lock_guard<decltype(mx)> lock(mx); log(z); }


/* Abstract backend */
struct BackendCPU {
    int num;
    BackendCPU(int num) : num{num} {}
};

std::shared_ptr<BackendCPU> single_bck; 

namespace ngraph {
   namespace runtime {
       namespace Backend {


           /*

 38│ std::shared_ptr<runtime::Backend> runtime::Backend::create(const string& type,
 39│                                                            bool must_support_dynamic)
 40│ {
 41│     auto inner_backend = BackendManager::create_backend(type);
 42│
 43├    if (!must_support_dynamic || inner_backend->supports_dynamic_tensors())
 44│     {
 45│         return inner_backend;
 46│     }
 47│     else
 48│     {
 49│         return make_shared<runtime::dynamic::DynamicBackend>(inner_backend);
 50│     }
 51│ }

           */

               std::shared_ptr<BackendCPU> create(const char *name="CPU")
               {
                  
                  
                 //  return single_bck;

                   return std::make_shared<BackendCPU>(2);
               }
           
       }
   }
}



struct NgraphEngine {

          static std::weak_ptr<BackendCPU> wp_backend_;
          std::shared_ptr<BackendCPU> backend_;

          
          void run() {
               

                  if(!wp_backend_.lock())
                  {
                         backend_ = ngraph::runtime::Backend::create("CPU");
                         wp_backend_ = backend_; 
                  }
                  else
                  {
                        backend_ = wp_backend_.lock();
                  }
                  
                  
          }

};

std::weak_ptr<BackendCPU> NgraphEngine::wp_backend_;
std::size_t num_threads = 0;





int main(int argc, char **argv) {

   if(argc!=2)
   {
      log_err("error : ./prog [thread_num]");
   }

   {
       std::stringstream s;
       s << argv[1];
       s >> num_threads;
       if(s.fail() || !num_threads )
       {
           log_err("Incorrect args " << argv[1]);
       }       
      
   }
 
   single_bck = std::make_shared<BackendCPU>(2);

   log("Run test with " << num_threads  << " threads");
   
   std::mutex mx;
   std::vector<std::thread> v;
   std::size_t thread_counter = 0;
   for(size_t i=0;i<num_threads;++i)
   
   v.emplace_back([&mx,&thread_counter](){
          log_sync(mx, "start thread " << (thread_counter++) << " id=" << std::this_thread::get_id() );
        
             
            for(size_t i=0;i<1000;++i)
            {      
                   NgraphEngine ng;  
                   ng.run();
            }


   });

   
   for(auto& _thread : v)
           _thread.join();

    log("End of test :)");

    return 0;
}
