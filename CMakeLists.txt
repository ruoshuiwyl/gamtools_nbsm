cmake_minimum_required(VERSION 3.5)
project(gamtools_sm)

set(CMAKE_CXX_COMPILER /usr/bin/g++-7)
#set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK" )
set(CMAKE_CXX_FLAGS  "-DBOOST_ALL_DYN_LINK -D_GLIBCXX_USE_CXX11_ABI=0")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)



option(SM_BUILD_TESTS "Build GAMTOOLS SM's unit tests" ON)

include_directories(
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1"
#        "${PROJECT_SOURCE_DIR}/lib/bwa-0.7.15"
)

link_libraries( "${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1"
#        "${PROJECT_SOURCE_DIR}/lib/bwa-0.7.15"
        )


add_library(nbsm ""
        sharding/gam_block.cc
        sharding/gam_block.h
        sharding/bam_partition_data.h
        sharding/bam_partition_data.cc
        sharding/bam_block.cc
        sharding/bam_block.h)
target_sources(
        nbsm
        PRIVATE
        "${PROJECT_SOURCE_DIR}/util/skiplist.h"
        "${PROJECT_SOURCE_DIR}/util/slice.h"
        "${PROJECT_SOURCE_DIR}/util/channel.h"
        "${PROJECT_SOURCE_DIR}/util/arena.cc"
        "${PROJECT_SOURCE_DIR}/util/arena.h"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.h"
        "sharding/bam_sharding_impl.cc"
        "sharding/bam_sharding_impl.h"
        "sharding/bam_partition_data.cc"
        "sharding/bam_partition_data.h"
        "${PROJECT_SOURCE_DIR}/util/random.h"
        "${PROJECT_SOURCE_DIR}/util/atomic_ptr.h"

)
add_library(gam_sm ""
        util/create_index.cc
        util/create_index.h
        fastqc/filter_processor.cc
        fastqc/filter_processor.h
        fastqc/base_fastq_info.cc
        fastqc/base_fastq_info.h
        util/gam_read_buffer.cc
        util/gam_read_buffer.h
        mkdup/mark_duplicate_frag_end.cc
        mkdup/mark_duplicate_frag_end.h
        fastqc/gam_fastq_file.cc
        fastqc/gam_fastq_file.h
        util/nbsm_static.h
        util/nbsm_static.cc
        fastqc/gam_fastq_read_impl.cc
        fastqc/gam_fastq_read_impl.h
        bwa_mem/gam_bwa_mem.cc
        bwa_mem/gam_bwa_mem.h
        nbsm/nbsm_impl.cc
        nbsm/nbsm_impl.h
        nbsm/nbsm_options.cc
        nbsm/nbsm_options.h
        util/glogger.cc
        util/glogger.h
        nbsm/options.h)

target_sources(gam_sm PRIVATE
        "${PROJECT_SOURCE_DIR}/util/slice.h"
        "${PROJECT_SOURCE_DIR}/util/arena.cc"
        "${PROJECT_SOURCE_DIR}/util/arena.h"
        "util/block.cc"
        "util/block.h"
        "sharding/gam_block.cc"
        "sharding/gam_block.h"

        "${PROJECT_SOURCE_DIR}/sharding/bam_sharding_impl.cc"
        "${PROJECT_SOURCE_DIR}/sharding/bam_sharding_impl.h"
        "sharding/bam_partition_data.h"
        "sharding/bam_partition_data.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/gam_mark_duplicate_impl.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/gam_mark_duplicate_impl.h"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_region.h"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_region.cc"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_read_ends.h"
        "${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_read_ends.cc"
        "${PROJECT_SOURCE_DIR}/util/gam_thread_pool.h"
        "${PROJECT_SOURCE_DIR}/util/gam_thread_pool.cc"

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

        "${PROJECT_SOURCE_DIR}/util/glogger.h"
        "${PROJECT_SOURCE_DIR}/util/glogger.cc"

        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.h"
        "${PROJECT_SOURCE_DIR}/nbsm/options.cc"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.h"
        "${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl.cc"

        #

        )


#add_executable(gamtools_sm nbsm/main.cc ${SOURCE_FILES})
#target_link_libraries(gamtools_sm)

if(SM_BUILD_TESTS)
    enable_testing()
    function(sm_test test_file)
        get_filename_component(test_target_name "${test_file}" NAME_WE)
        add_executable("${test_target_name}" "")
        target_sources("${test_target_name}"
                PRIVATE

                "${test_file}"
                )
        target_link_libraries("${test_target_name}" gam_sm gtest pthread snappy z ${PROJECT_SOURCE_DIR}/lib/htslib-1.3.1/libhts.a)
        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
    endfunction(leveldb_test)

    sm_test("${PROJECT_SOURCE_DIR}/util/skiplist_test.cc")
    sm_test("sharding/bam_block_test.cc")
    sm_test("sharding/bam_block_sort_test.cc")
    sm_test("sharding/bam_partition_data_test.cc")
    sm_test("${PROJECT_SOURCE_DIR}/nbsm/gamtools_sm_impl_test.cc")
#    sm_test("${PROJECT_SOURCE_DIR}/bwa_mem/bwa_mem_test.cc")
    sm_test("${PROJECT_SOURCE_DIR}/mkdup/mark_duplicate_region_test.cc")
    sm_test("${PROJECT_SOURCE_DIR}/nbsm/nbsm_impl_test.cc")
#    sm_test("${PROJECT_SOURCE_DIR}/util/glogger_test.cc")


endif(SM_BUILD_TESTS)

#add_executable(glogger_test util/glogger_test.cc util/glogger.h util/glogger.cc)
#target_link_libraries(glogger_test boost_log pthread boost_thread boost_system boost_log_setup)

#add_executable(nbsm_options_test nbsm/nbsm_options_test.cc nbsm/nbsm_options.h nbsm/nbsm_options.cc util/glogger.h util/glogger.cc)
#target_link_libraries(nbsm_options_test boost_program_options boost_log pthread boost_thread boost_system boost_log_setup)
target_link_libraries(nbsm_impl_test boost_program_options boost_log pthread boost_thread boost_system boost_log_setup)