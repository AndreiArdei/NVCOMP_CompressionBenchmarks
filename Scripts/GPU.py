import sys
import subprocess

if len(sys.argv) < 3:
    print ("Usage: python3 GPU.py <iteartions> <outputfile> <binary input filename(s)>")
    sys.exit(0)

iterations= sys.argv[1]
outputfile = sys.argv[2]
CHUNKS = [2048, 4096, 8192, 16384, 32792, 65536]
log = open(outputfile + '.log', 'w')

for i in range(3,len(sys.argv)):
    filename = sys.argv[i]
    print ("Starting benchmark on file:", filename)

    snappy, bitcomp, ans, cascaded, gdeflate, lz4 = 0, 0, 0, 0, 0, 0

    for chunk in CHUNKS:
        try:
            cmd = '~/GPUTest/bin/benchmark_ans_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            snappy = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('ANS decompression FAILED with chunks = ' + str(chunk)+ '\r\n')
                
        try:
            cmd = '~/GPUTest/bin/benchmark_bitcomp_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            bitcomp = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('Bitcomp decompression FAILED with chunks = ' + str(chunk)+ '\r\n')

        try:
            cmd = '~/GPUTest/bin/benchmark_cascaded_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            ans = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('Cascaded decompression FAILED with chunks = ' + str(chunk)+ '\r\n')


        try:
            cmd = '~/GPUTest/bin/benchmark_deflate_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            cascaded = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('Deflate decompression FAILED with chunks = ' + str(chunk)+ '\r\n')

        try:
            cmd = '~/GPUTest/bin/benchmark_gdeflate_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            gdeflate = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('GDeflate decompression FAILED with chunks = ' + str(chunk) + '\r\n')


        try:
            cmd = '~/GPUTest/bin/benchmark_lz4_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            lz4 = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('LZ4 decompression FAILED with chunks = ' + str(chunk)+ '\r\n')

        try:
            cmd = '~/GPUTest/bin/benchmark_snappy_chunked -f ' + str(filename) + ' -w 500 -i ' + str(iterations) + ' -p ' + str(chunk)
            log.write(cmd + "\r\n")
            result = subprocess.check_output(cmd, shell=True)
            log.write(str(result) + "\r\n")
            lz4 = result.split()[-1]
        except subprocess.CalledProcessError:
            log.write('Snappy decompression FAILED with chunks = ' + str(chunk)+ '\r\n')
