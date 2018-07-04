#!/usr/bin/python3
import psutil
import time
# from subprocess import PIPE
import shlex
import datetime
import sys
args = ""


# args = "/home/ruoshui/projects/gamtools_nbsm/cmake-build-debug/fastaln -r /home/ruoshui/data/na12878/chr21_5w/test_reference/test.fa -d /home/ruoshui/logs/chr21_5w -f /home/ruoshui/data/na12878/chr21_5w/chr21_9411000_9459000_name_id_read1.fq -b /home/ruoshui/data/na12878/chr21_5w/chr21_9411000_9459000_name_id_read2.fq -l 0 -a 0 -f /home/ruoshui/data/na12878/chr21_5w/chr21_9411000_9459000_name_id_read1.fq -b /home/ruoshui/data/na12878/chr21_5w/chr21_9411000_9459000_name_id_read2.fq -l 0 -a 1 -o /home/ruoshui/data/test.bam -n zerba -g foo -t 1 -B /home/ruoshui/data/na12878/chr21_5w/test_reference/test.bed"
commands = shlex.split(args)
print(commands)
p = psutil.Popen(commands)
cnt = 0
tmp_file = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
prof_file = tmp_file + "_prof.txt"
f = open (prof_file, "w")
while True :
    memory_rss = p.memory_info().rss / (1024 * 1024 * 1024)
    f.write('%s %d %.2f %.2f %.2f\n' %(p.name(), cnt, p.cpu_percent(), p.memory_percent(), memory_rss ))
    time.sleep(5)
    cnt = cnt + 5
    returnCode = p.poll()
    print ("returnCode:", returnCode)
    if returnCode == 0 :
        f.close()
        break