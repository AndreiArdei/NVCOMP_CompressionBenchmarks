from datetime import datetime
import sys
import subprocess

if len(sys.argv) < 2:
    print ("Usage: python3 GPU.py <iteartions> <outputfile>")
    sys.exit(0)

iterations= sys.argv[1]
outputfile = sys.argv[2]
CHUNKS = [2, 4, 8, 16, 32, 64]
ALGORITHMS = ['ans', 'bitcomp', 'cascaded', 'deflate', 'gdeflate', 'lz4', 'snappy']
FILES = ["/var/scratch/alvarban/CERN_data/muonData/S-Muon_cleanmask-0/cluster120_Muon_cleanmask-0_pg0.page", 
 	"/var/scratch/alvarban/CERN_data/muonData/S-Muon_miniIsoId-0/cluster24_Muon_miniIsoId-0_pg0.page", 
	"/var/scratch/alvarban/CERN_data/muonData/I-Muon_charge-0/cluster12_Muon_charge-0_pg0.page", 
	"/var/scratch/alvarban/CERN_data/muonData/I-Muon_nTrackerLayers-0/cluster62_Muon_nTrackerLayers-0_pg0.page", 
	"/var/scratch/alvarban/CERN_data/muonData/F-Muon_dxy-0/cluster21_Muon_dxy-0_pg0.page", 
	"/var/scratch/alvarban/CERN_data/muonData/F-Muon_dxyErr-0/cluster14_Muon_dxyErr-0_pg0.page", 
	"/var/scratch/alvarban/CERN_data/muonData/F-Muon_eta-0/cluster25_Muon_eta-0_pg0.page", 
	"/var/scratch/alvarban/CERN_data/muonData/B-Muon_isGlobal-0/cluster81_Muon_isGlobal-0_pg0.page",
	"/var/scratch/alvarban/CERN_data/muonData/B-Muon_mediumId-0/cluster20_Muon_mediumId-0_pg0.page"]
log = open(outputfile + '.log', 'w')
for ALGO in ALGORITHMS:
    for chunk in CHUNKS:
        chunkStart = datetime.now()
        for file in FILES:
            try:
                cmd = '~/GPUTest/bin/benchmark_hlif '+ str(ALGO) + ' -f '+ str(file) +'  -n ' + str(iterations) + ' -c ' + str(chunk)
                log.write(cmd + "\r\n")
                startTime = datetime.now()
                result = subprocess.check_output(cmd, shell=True)
                stopTime = datetime.now()
                log.write(str(result) + "\r\n")
                log.write("Execution time : " + str(stopTime - startTime) + " seconds")
            except subprocess.CalledProcessError:
                log.write(str(ALGO) + ' decompression FAILED with chunks = ' + str(chunk)+ '\r\n')
        log.write("Execution time for ALL - " + str(datetime.now() - chunkStart))

