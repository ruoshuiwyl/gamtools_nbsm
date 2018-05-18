//
// Created by ruoshui on 5/16/18.
//

#ifndef GAMTOOLS_SM_GLOGGER_H
#define GAMTOOLS_SM_GLOGGER_H


//#ifndef BOOST_LOG
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/exception/all.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <fstream>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

//#endif

namespace gamtools {
//#ifdef  BOOST_LOG
    enum sign_severity_level {
        trace,
        debug,
        info,
        warning,
        error,
        fatal,
        report
    };

    template< typename CharT, typename TraitsT >
    inline std::basic_ostream< CharT, TraitsT >& operator<< (
            std::basic_ostream< CharT, TraitsT >& strm, sign_severity_level lvl)  {
        static const char* const str[] =   {
                "Trace",
                "Debug",
                "Info",
                "Warning",
                "Error",
                "Fatal",
                "Report"
                };
        if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
            strm << str[lvl];
        else
            strm << static_cast< int >(lvl);
        return strm;
    }



    BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::severity_logger_mt<sign_severity_level>)

    class GLogger {
    public:
        static src::severity_logger_mt<gamtools::sign_severity_level> glog;
        static void InitLog(std::string &log_dir);
        static void SetLevel(sign_severity_level sl);
    };
//#endif

#ifdef BOOST_LOG
#define GLOG_REPORT  BOOST_LOG_SEV(GLogger::glog,report)
#define GLOG_INFO    BOOST_LOG_SEV(GLogger::glog,info)
#define GLOG_TRACE   BOOST_LOG_SEV(GLogger::glog,trace)
#define GLOG_WARNING BOOST_LOG_SEV(GLogger::glog,warning)
#define GLOG_ERROR   BOOST_LOG_SEV(GLogger::glog,error)
#define GLOG_DEBUG   BOOST_LOG_SEV(GLogger::glog,debug)
#define GLOG_FATAL   BOOST_LOG_SEV(GLogger::glog,fatal)
#else
#define GLOG_REPORT  std::clog<<"Report: "
#define GLOG_INFO    std::clog<<"Info: "
#define GLOG_TRACE   std::clog<<"Trace: "
#define GLOG_WARNING std::clog<<"Waring: "
#define GLOG_ERROR   std::clog<<"Error: "
#define GLOG_DEBUG   std::clog<<"Debug: "
#define GLOG_FATAL   std::clog<<"Fatal: "
#endif

}


#endif //GAMTOOLS_SM_GLOGGER_H
