# fastAln


## Description

### Filter
fastAln base on soapnuke
fiter low quality
filter n_nate
filter have a adapter
### Align
fastAln base on bwa(0.7.15) mem align
### Sort
fastAln base on samtools sort algorithm;
### MarkDuplicate
fastAln base on PicardTools MarkDuplicate
Algorithm :
compute the sum of quality score of read >= 15, Mark Duplicate low score read And the same score may mark duplicate read input order


## Install
### dependent environment
gcc support c++11 recomment gcc 5.0 +
boost 1.60.0 +
snappy

### Install
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 20 fastAln
## Usage

bwa index hg19.fa
Single fastq files
./fastAln -r hg19.fa  -d ./tmp -f  read1.fq -b read2.fq  -l 0 -a 0 -o test.bam -n zerba -i foo -t 1
Multiple pe files
./fastAln -r hg19.fa  -d ./tmp -f  read1_1.fq -b read1_2.fq  -l 0 -a 0 -f read2_1.fq -b read2_2.fq -l 0 -a 1 -o test.bam -n zerba -i foo -t 1
## parameter

  - -h [ --help ]                        Print CheckHelp message
  - -v [ --version ]                     Print version message
  - -r [ --reference_file ] arg          Reference sequence file Default(null)
  - -d [ --temp_dir ] arg                Temporary directory Storage space size
                                       must be twice BAM file size
                                       Default(null)
  - -f [ --input_fastq1_lists ] arg      Input fastq1 file lists
  - -b [ --input_fastq2_lists ] arg      Input fastq2 file lists
  - -l [ --input_library_id_lists ] arg  Input fastq file library ID
  - -a [ --input_lane_id_lists ] arg     Input fastq file lane ID
  - -o [ --output_bam_file ] arg         Output bam file name defalut(null)
  - -n [ --sample_name ] arg (=Zebra)    Sample Name Default(Zebra)
  - -i [ --sample_id ] arg (=Zebra)      Sample Name Default(Zebra)
  - -t [ --thread_number ] arg (=1)      NBSM total thread Default(1)
  - -B [ --bed_file ] arg
  - --batch_size arg (=100000)           NBSM process batch read size Default(20
                                       * 10000000)
  - --read_len arg (=100)                NBSM process read length Default(100)

  - --filter_low_qual arg                Filter Read low quality threshold
                                       Default(10)
  - --filter_qual_rate arg               Filter Read low quality rate Default(0.5
  - --filter_n_nate arg                  Filter Read N rate threshold Default
                                       (0.05)
  - --filter_adapter1 arg                3' adapter sequence Default(AAGTCGGAGGCC
                                       AAGCGGTCTTAGGAAGACAA)
  - --filter_adapter2 arg                5' adapter sequence Default(AAGTCGGATCGT
                                       AGCCATGTCGTTCTGTGAGCCAAGGAGTTG)
  - --filter_mis_match arg               the max mismatch number when match the
                                       adapter Default(1)
  - --filter_match_ratio arg             adapter's shortest match ratio
                                       Default(0.5)
  - --filter_qual_system arg             quality system 1:illumina, 2:sanger
                                       Default(2:illumina)

  - --bwamem_thread arg                  bwa mem number of threads Default(nbsm
                                       thread)
  - --bawmem_min_seed arg                bwa mem minimum seed length Default(19)
  - --bwamem_band_width arg              bwa mem band width for banded alignment
                                       Default(100)
  - --bwamem_x_dropoff arg               bwa mem off-diagonal X-dropoff
                                       Default(100)
  - --bwamem_internal_seed arg           look for internal seeds inside a seed
                                       longer than {-k} * FLOAT Defulat(1.5)
  - --bwamem_skip_number arg             skip seeds with more than INT
                                       occurrences Default(500)
  - --bwamem_drop_chain arg              drop chains shorter than FLOAT fraction
                                       of the longest overlapping chain
                                       Defulat(0.50)
  - --bwamem_discard_len arg             discard a chain if seeded bases shorter
                                       than INT Defalut(0)
  - --bwamem_max_rounds arg              perform at most INT rounds of mate
                                       rescues for each read Default(50)
  - --bwamem_skip_mate_rescue arg        bwa mem skip mate rescue
  - --bwamem_skip_pairing arg            bwa mem skip pairing; mate rescue
                                       performed unless -S also in use
  - --bwamem_match_score arg             bwa mem score for a sequence match
                                       Default(1)
  - --bwamem_minmatch_score arg          bwa mem  penalty for a mismatch
                                       Default(4)
  - --bwamem_gap_open_penalties arg      gap open penalties for deletions and
                                       insertions Default[6,6]
  - --bwamem_gap_extension_score arg     gap extension penalty; a gap of size k
                                       cost '{-O} + {-E}*k' Default[1,1]
  - --bwamem_clipping_penalty arg         penalty for 5'- and 3'-end clipping
                                       Default[5,5]
  - --bwamem_unpair_penalty arg          penalty for an unpaired read pair
                                       Default(17)

  - --sm_sort_sharding_size arg          Sort Sharding size Default(4M)
  - --sm_sort_block_size arg             sort region buffer size to save gam
                                       record Default(1M)
  - --sm_mkdup_sharding_size arg         Mark Duplicate Sharding size Default(8M)
  - --sm_mkdup_block_size arg            Mark Duplicate block buffer size for to
                                       save mkdup info Default(1M)
  - --sm_block_sort_thread arg           sharding stage block sort thread number
                                       Default(4)
  - --sm_read_gam_thread_num arg         Merge stage read sharding data thread
                                       number Default(1)
  - --sm_merge_thread arg                Merge sort thread num Default(1)
  - --sm_compress_bam_thread arg         Compress bam thread number Default(1)
  - --sm_mkdup_thread arg                Mark Duplicate thread number Default (1)

## Performance and Accuracy

###Performance
ENV : centos7 128G 2*12 E3 CPU
DataSets: NA12878 BGISEQ500(38X)
Time : 7h vs
Accuracy:
filter:
mapped rate:
markdup rate:

## QC Report



### Filter uniquely mapped reads
[ Mappability (Mapping Quality) Score Of Unique Aligned Reads](https://www.biostars.org/p/59282/)


## Limitations
- fastAln requires huge memory
- fastAln only processing single library datasets




