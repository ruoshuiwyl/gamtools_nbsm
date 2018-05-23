//
// Created by ruoshui on 5/16/18.
//
#include "glogger.h"

using  namespace gamtools;
//void Bar() {
//    int x = 0;
//}
//
//void Error() {
//    BOOST_LOG_FUNCTION();
//
//    // Attach a copy of the current scope stack to the exception
//    throw boost::enable_error_info(std::range_error("test exception")) << logging::current_scope();
//}
//
//
//void Foo(int n) {
////    src::severity_logger_mt<gamtools::sign_severity_level>& lg = my_logger::get();
//    // Mark the scope of the function foo
//    BOOST_LOG_FUNCTION();
//
//    switch (n)
//    {
//        case 0:
//        {
//            // Mark the current scope
//            BOOST_LOG_NAMED_SCOPE("case 0");
//            BOOST_LOG_SEV(lg, trace) << "Some log record";
//            try {
//                Error();
//            }
//            catch (std::range_error& e) {
//                // Acquire the scope stack from the exception object
//                BOOST_LOG_SEV(lg, trace) << "Error call failed: " << e.what() << ", scopes stack:\n"
//                                         << *boost::get_error_info<logging::current_scope_info>(e);
//            }
//        }
//            break;
//
//        case 1:
//        {
//            // Mark the current scope
//            BOOST_LOG_NAMED_SCOPE("case 1");
//            BOOST_LOG_SEV(lg, trace) << "Some log record";
//            Bar(); // call some function
//        }
//            break;
//
//        default:
//        {
//            // Mark the current scope
//            BOOST_LOG_NAMED_SCOPE("default");
//            BOOST_LOG(lg) << "Some log record";
//            Bar(); // call some function
//        }
//            break;
//    }
//}

int main(int, char*[]) {
    std::string log_dir = "/home/ruoshui/work/gamtools_sm/cmake-build-debug";
    GLogger::InitLog(log_dir);
    BOOST_LOG_FUNCTION();
    GLOG_DEBUG << "A debug severity message";
    GLOG_INFO << "A info  severity message";
    GLOG_TRACE << "A trace severity message";
    GLOG_WARNING << "A warning severity message";
    GLOG_ERROR << "A Error severity message";
    GLOG_REPORT << "A report severity message";
    GLOG_FATAL << "A fatal severity message";
//    src::severity_logger_mt<sign_severity_level>& lg = my_logger::get();
//    BOOST_LOG_SEV(lg, trace) << "A trace severity message";
//    BOOST_LOG_SEV(lg, debug) << "A debug severity message";
//    BOOST_LOG_SEV(lg, info) << "An informational severity message";
//    BOOST_LOG_SEV(lg, warning) << "A warning severity message";
//    BOOST_LOG_SEV(lg, error) << "An error severity message";
//    BOOST_LOG_SEV(lg, fatal) << "A fatal severity message";
//    BOOST_LOG_SEV(lg, report) << "A report severity message";

//    Foo(1);
//    Foo(0);

    logging::core::get()->remove_all_sinks();
    return 0;
}


