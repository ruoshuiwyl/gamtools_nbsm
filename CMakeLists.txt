cmake_minimum_required(VERSION 3.5)
project(fastaln)

set(CMAKE_CXX_COMPILER /usr/bin/g++-7)
#set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK" )
#set(CMAKE_CXX_FLAGS  "-DBOOST_ALL_DYN_LINK -D_GLIBCXX_USE_CXX11_ABI=0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DBOOST_ALL_DYN_LINK -D_GLIBCXX_USE_CXX11_ABI=0 -DDEBUG -DTESTDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DBOOST_ALL_DYN_LINK -D_GLIBCXX_USE_CXX11_ABI=0 -O2 ")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(Boost_INCLUDE_DIR /usr/include)
set(Boost_LIBRARY_DIR /usr/lib64)
find_package(Boost COMPONENTS  thread system filesystem  program_options log log_setup REQUIRED)

option(SM_BUILD_TESTS "Build GAMTOOLS SM's unit tests" OFF)

include_directories(
        "${Boost_INCLUDE_DIR}"
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/hts"
#        "${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1"
#        "${PROJECT_SOURCE_DIR}/lib/bwa-0.7.15"
)

link_libraries(
        ${Boost_LIBRARY_DIR}
#        "${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1"
#        "${PROJECT_SOURCE_DIR}/lib/bwa-0.7.15"
        )

set( FASTALN_SOURCE
        "${PROJECT_SOURCE_DIR}/bwa_mem/kstring.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/kstring.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwa.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwa.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwt.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwt.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/utils.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/utils.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/kthread.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bntseq.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bntseq.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/ksw.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/ksw.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem_pair.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem_extra.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/QSufSort.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/QSufSort.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/gam_bwa_mem.cc"
        "${PROJECT_SOURCE_DIR}/bwa_mem/gam_bwa_mem.h"
#

        "${PROJECT_SOURCE_DIR}/hts/bgzf.c"
        "${PROJECT_SOURCE_DIR}/hts/hfile.c"
        "${PROJECT_SOURCE_DIR}/hts/hfile_net.c"
        "${PROJECT_SOURCE_DIR}/hts/hts.c"
        "${PROJECT_SOURCE_DIR}/hts/sam.c"
        "${PROJECT_SOURCE_DIR}/hts/knetfile.c"

        "${PROJECT_SOURCE_DIR}/util/slice.h"
        "${PROJECT_SOURCE_DIR}/util/arena.cc"
        "${PROJECT_SOURCE_DIR}/util/arena.h"
        "${PROJECT_SOURCE_DIR}/util/block.cc"
        "${PROJECT_SOURCE_DIR}/util/block.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.cc"
        "${PROJECT_SOURCE_DIR}/util/create_index.cc"
        "${PROJECT_SOURCE_DIR}/util/create_index.h"
        "${PROJECT_SOURCE_DIR}/util/nbsm_static.h"
        "${PROJECT_SOURCE_DIR}/util/nbsm_static.cc"
        "${PROJECT_SOURCE_DIR}/util/gam_read_buffer.cc"
        "${PROJECT_SOURCE_DIR}/util/gam_read_buffer.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.cc"
        "${PROJECT_SOURCE_DIR}/util/glogger.h"
        "${PROJECT_SOURCE_DIR}/util/debug_util.cc"
        "${PROJECT_SOURCE_DIR}/util/debug_util.h"
        "${PROJECT_SOURCE_DIR}/util/bounded_queue.h"
        "${PROJECT_SOURCE_DIR}/util/block_queue.h"
        "${PROJECT_SOURCE_DIR}/util/order_block_queue.h"
        "${PROJECT_SOURCE_DIR}/sharding/gam_block.cc"
        "${PROJECT_SOURCE_DIR}/sharding/gam_block.h"
        "${PROJECT_SOURCE_DIR}/sharding/bam_block.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_block.h"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sharding_impl.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_partition_data.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sort_mkdup_impl.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sort_mkdup_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_options.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_options.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.h"
        "${PROJECT_SOURCE_DIR}/mkdup/gam_mark_duplicate_impl.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_region.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_read_ends.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_compute.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_result_analysis.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_result_data.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_stat_impl.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_file.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_file.h"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_read_impl.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_read_impl.h"
        "${PROJECT_SOURCE_DIR}/fastqc/filter_processor.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/filter_processor.h"
        "${PROJECT_SOURCE_DIR}/fastqc/base_fastq_info.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/base_fastq_info.h"
        "${PROJECT_SOURCE_DIR}/fastqc/fastq_info_statistics.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/fastq_info_statistics.h"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_frag_end.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_frag_end.h"
        )

add_library(gam_sm ""
        "${PROJECT_SOURCE_DIR}/bwa_mem/gam_bwa_mem.cc"
        "${PROJECT_SOURCE_DIR}/bwa_mem/gam_bwa_mem.h"
        "${PROJECT_SOURCE_DIR}/util/create_index.cc"
        "${PROJECT_SOURCE_DIR}/util/create_index.h"
        "${PROJECT_SOURCE_DIR}/util/nbsm_static.h"
        "${PROJECT_SOURCE_DIR}/util/nbsm_static.cc"
        "${PROJECT_SOURCE_DIR}/util/gam_read_buffer.cc"
        "${PROJECT_SOURCE_DIR}/util/gam_read_buffer.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.cc"
        "${PROJECT_SOURCE_DIR}/util/glogger.h"
        "${PROJECT_SOURCE_DIR}/util/debug_util.cc"
        "${PROJECT_SOURCE_DIR}/util/debug_util.h"
        "${PROJECT_SOURCE_DIR}/util/bounded_queue.h"
        "${PROJECT_SOURCE_DIR}/util/block_queue.h"
        "${PROJECT_SOURCE_DIR}/util/order_block_queue.h"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_file.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_file.h"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_read_impl.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/gam_fastq_read_impl.h"
        "${PROJECT_SOURCE_DIR}/fastqc/filter_processor.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/filter_processor.h"
        "${PROJECT_SOURCE_DIR}/fastqc/base_fastq_info.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/base_fastq_info.h"
        "${PROJECT_SOURCE_DIR}/fastqc/fastq_info_statistics.cc"
        "${PROJECT_SOURCE_DIR}/fastqc/fastq_info_statistics.h"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_frag_end.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_frag_end.h"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_options.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_options.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.h"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sort_mkdup_impl.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sort_mkdup_impl.h"
       )

target_sources(gam_sm PRIVATE
        "${PROJECT_SOURCE_DIR}/util/slice.h"
        "${PROJECT_SOURCE_DIR}/util/arena.cc"
        "${PROJECT_SOURCE_DIR}/util/arena.h"
        "${PROJECT_SOURCE_DIR}/util/block.cc"
        "${PROJECT_SOURCE_DIR}/util/block.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.cc"
        "${PROJECT_SOURCE_DIR}/util/create_index.cc"
        "${PROJECT_SOURCE_DIR}/sharding/gam_block.cc"
        "${PROJECT_SOURCE_DIR}/sharding/gam_block.h"
        "${PROJECT_SOURCE_DIR}/sharding/bam_block.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_block.h"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sharding_impl.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_partition_data.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/gam_mark_duplicate_impl.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_region.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_read_ends.cc"

        "${PROJECT_SOURCE_DIR}/bwa_mem/kstring.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/kstring.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwa.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwa.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwt.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwt.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/utils.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/utils.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/kthread.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bntseq.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bntseq.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/ksw.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/ksw.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem_pair.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/bwamem_extra.c"
        "${PROJECT_SOURCE_DIR}/bwa_mem/QSufSort.h"
        "${PROJECT_SOURCE_DIR}/bwa_mem/QSufSort.c"

        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_compute.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_result_analysis.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_result_data.cc"
        "${PROJECT_SOURCE_DIR}/qc_report/qc_stat_impl.cc"
        #

        )


#add_executable(gamtools_sm nbsm/main.cc ${SOURCE_FILES})
#target_link_libraries(gamtools_sm)

#if(SM_BUILD_TESTS)
#    enable_testing()
#    function(sm_test test_file)
#        get_filename_component(test_target_name "${test_file}" NAME_WE)
#        add_executable("${test_target_name}" "")
#        target_sources("${test_target_name}"
#                PRIVATE
#
#                "${test_file}"
#                )
#        target_link_libraries("${test_target_name}" gam_sm gtest pthread snappy z ${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1/libhts.a)
#        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
#    endfunction(leveldb_test)
#
#    sm_test("${PROJECT_SOURCE_DIR}/util/skiplist_test.cc")
#    sm_test("sharding/bam_block_test.cc")
#    sm_test("sharding/bam_block_sort_test.cc")
#    sm_test("sharding/bam_partition_data_test.cc")
#    sm_test("${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl_test.cc")
##    sm_test("${PROJECT_SOURCE_DIR}/bwa_mem/bwa_mem_test.cc")
#    sm_test("${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_region_test.cc")
#    sm_test("${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl_test.cc")
#    sm_test("${PROJECT_SOURCE_DIR}/qc_report/qc_test.cc")
##    sm_test("${PROJECT_SOURCE_DIR}/util/glogger_test.cc")
#
#
#endif(SM_BUILD_TESTS)

#add_executable(glogger_test util/glogger_test.cc util/glogger.h util/glogger.cc)
#target_link_libraries(glogger_test boost_log pthread boost_thread boost_system boost_log_setup)

#add_executable(nbsm_options_test nbsm/nbsm_options_test.cc nbsm/nbsm_options.h nbsm/nbsm_options.cc util/glogger.h util/glogger.cc)
#target_link_libraries(nbsm_options_test boost_program_options boost_log pthread boost_thread boost_system boost_log_setup)
#target_link_libraries(nbsm_impl_test boost_program_options boost_log pthread boost_thread boost_system boost_log_setup boost_filesystem)
#add_executable(qc_test qc_report/qc_test.cc qc_report/qc_compute.cc qc_report/qc_stat_impl.cc util/glogger.cc)
#target_link_libraries(qc_test ${Boost_LIBRARIES} pthread)

add_executable(fastaln nbsm/main.cc ${FASTALN_SOURCE})
#target_link_libraries(fastaln  pthread ${Boost_LIBRARIES} ${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1/libhts.a z snappy )
target_link_libraries(fastaln  pthread ${Boost_LIBRARIES}  z snappy )

add_executable(bam_out nbsm/bam_output_test.cc )
target_link_libraries(bam_out /home/ruoshui/code/samtools/samtools-1.3.1/htslib-1.3.1/libhts.a z pthread)