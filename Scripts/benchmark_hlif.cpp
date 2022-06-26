/*
 * Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Benchmark performance from the binary data file fname
#include <vector>
#include <string.h>

#include "benchmark_common.h"
#include "nvcomp.hpp"
#include "nvcomp/nvcompManagerFactory.hpp"
#include "benchmark_hlif.hpp"

#define N 8
#define TIMING
//#define SERIAL
#define PARALLEL

using namespace nvcomp;

void run_benchmark_from_file(char* fname, nvcompManagerBase& batch_manager, int verbose_memory, cudaStream_t stream, const int benchmark_exec_count)
{
  using T = uint8_t;

  size_t input_elts = 0;
  std::vector<T> data;
  data = load_dataset_from_binary<T>(fname, &input_elts);
  run_benchmark(data, batch_manager, verbose_memory, stream, benchmark_exec_count);
}

static void print_usage()
{
  printf("Usage: benchmark_hlif [format_type] [OPTIONS]\n");
  printf("  %-35s One of <snappy / bitcomp / ans / cascaded/ gdeflate / lz4>\n", "[ format_type ]");
  printf("  %-35s Binary dataset filename (required).\n", "-f, --filename");
  printf("  %-35s Chunk size (default 64 kB).\n", "-c, --chunk-size");
  printf("  %-35s GPU device number (default 0)\n", "-g, --gpu");
  printf("  %-35s Number of times to execute the benchmark (for averaging) (default 1)\n", "-n, --num-iters");
  printf("  %-35s Data type (default 'char', options are 'char', 'short', 'int')\n", "-t, --type");
  printf(
      "  %-35s Output GPU memory allocation sizes (default off)\n",
      "-m --memory");
  exit(1);
}

int main(int argc, char* argv[])
{
  char* fname = NULL;
  int gpu_num = 0;
  int verbose_memory = 0;
  int num_iters = 1;

  // Cascaded opts
  nvcompBatchedCascadedOpts_t cascaded_opts = nvcompBatchedCascadedDefaultOpts;

  // Shared opts
  int chunk_size = 1 << 16;
  nvcompType_t data_type = NVCOMP_TYPE_CHAR;

  std::string comp_format;

  bool explicit_type = false;
  bool explicit_chunk_size = false;

  // Parse command-line arguments
  char** argv_end = argv + argc;
  argv += 1;

  // First the format
  comp_format = std::string{*argv++};
  if (comp_format == "lz4") {
  } else if (comp_format == "snappy") {
  } else if (comp_format == "bitcomp") {
  } else if (comp_format == "ans") {
  } else if (comp_format == "cascaded") {
  } else if (comp_format == "gdeflate") {
  } else {
    printf("invalid format\n");
    print_usage();
    return 1;
  }

  while (argv != argv_end) {
    char* arg = *argv++;
    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-?") == 0) {
      print_usage();
      return 1;
    }
    if (strcmp(arg, "--memory") == 0 || strcmp(arg, "-m") == 0) {
      verbose_memory = 1;
      continue;
    }


    // all arguments below require at least a second value in argv
    if (argv >= argv_end) {
      print_usage();
      return 1;
    }

    char* optarg = *argv++;
    if (strcmp(arg, "--filename") == 0 || strcmp(arg, "-f") == 0) {
      fname = optarg;
      continue;
    }
    
    if (strcmp(arg, "--gpu") == 0 || strcmp(arg, "-g") == 0) {
      gpu_num = atoi(optarg);
      continue;
    }

    if (strcmp(arg, "--num-iters") == 0 || strcmp(arg, "-n") == 0) {
      num_iters = atoi(optarg);
      continue;
    }
    if (strcmp(arg, "--chunk-size") == 0 || strcmp(arg, "-c") == 0) {
      chunk_size = atoi(optarg);
      explicit_chunk_size = true;
      continue;
    }

    if (strcmp(arg, "--type") == 0 || strcmp(arg, "-t") == 0) {
      explicit_type = true;
      if (strcmp(optarg, "char") == 0) {
        data_type = NVCOMP_TYPE_CHAR;
      } else if (strcmp(optarg, "short") == 0) {
        data_type = NVCOMP_TYPE_SHORT;
      } else if (strcmp(optarg, "int") == 0) {
        data_type = NVCOMP_TYPE_INT;
      } else if (strcmp(optarg, "longlong") == 0) {
        data_type = NVCOMP_TYPE_LONGLONG;
      } else {
        print_usage();
        return 1;
      }
      continue;
    }

    if (strcmp(arg, "--num_rles") == 0 || strcmp(arg, "-r") == 0) {
      cascaded_opts.num_RLEs = atoi(optarg);
      continue;
    }
    if (strcmp(arg, "--num_deltas") == 0 || strcmp(arg, "-d") == 0) {
      cascaded_opts.num_deltas = atoi(optarg);
      continue;
    }
    if (strcmp(arg, "--num_bps") == 0 || strcmp(arg, "-b") == 0) {
      cascaded_opts.use_bp = (atoi(optarg) != 0);
      continue;
    }

    print_usage();
    return 1;
  }

  if (fname == NULL) {
    print_usage();
    return 1;
  }

  cudaSetDevice(gpu_num);

  cudaStream_t stream[N];

  for (int s=0; s<N; s++) 
  CUDA_CHECK(cudaStreamCreate(&stream[s]));

  std::vector<std::shared_ptr<nvcompManagerBase>> myManager(N);

 for (int s=0; s<N; s++) { 
//  std::shared_ptr<nvcompManagerBase> manager;
  
  if (comp_format == "lz4") {
    myManager[s] = std::make_shared<LZ4Manager>(chunk_size, data_type, stream[s], gpu_num, NoComputeNoVerify);
  } else if (comp_format == "snappy") {
    myManager[s] = std::make_shared<SnappyManager>(chunk_size, stream[s], gpu_num, NoComputeNoVerify);
  } else if (comp_format == "bitcomp") {
    myManager[s] = std::make_shared<BitcompManager>(data_type, 0 /* algo--fixed for now */, stream[s], gpu_num, NoComputeNoVerify);
  } else if (comp_format == "ans") {
    myManager[s] = std::make_shared<ANSManager>(chunk_size, stream[s], gpu_num, NoComputeNoVerify);
  } else if (comp_format == "cascaded") {
    if (explicit_type) {
      cascaded_opts.type = data_type;
    }

    if (explicit_chunk_size) {
      cascaded_opts.chunk_size = chunk_size;
    }

    myManager[s] = std::make_shared<CascadedManager>(cascaded_opts, stream[s], gpu_num, NoComputeNoVerify);
  } else if (comp_format == "gdeflate") {
    myManager[s] = std::make_shared<GdeflateManager>(chunk_size, 0 /* algo--fixed for now */, stream[s], gpu_num, NoComputeNoVerify);
  } else {
    print_usage();
    return 1;
  }
  //myManager.push_back(manager);
 }


/* std::vector<char*> fnames = {"../../CERN_data/cluster0_electrons-0_pg0.page", "../../CERN_data/cluster0_electrons-0_pg1.page","../../CERN_data/cluster1_electrons-0_pg0.page", "../../CERN_data/cluster1_electrons-0_pg1.page", "../../CERN_data/cluster2_electrons-0_pg0.page", "../../CERN_data/cluster2_electrons-0_pg1.page", "../../CERN_data/cluster3_electrons-0_pg0.page", "../../CERN_data/cluster3_electrons-0_pg1.page", "../../CERN_data/cluster4_electrons-0_pg0.page", "../../CERN_data/cluster4_electrons-0_pg1.page"};
*/

 std::vector<char*> fnames = {"/var/scratch/alvarban/CERN_data/muonData/S-Muon_cleanmask-0/cluster120_Muon_cleanmask-0_pg0.page", \
 	"/var/scratch/alvarban/CERN_data/muonData/S-Muon_miniIsoId-0/cluster24_Muon_miniIsoId-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/I-Muon_charge-0/cluster12_Muon_charge-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/I-Muon_nTrackerLayers-0/cluster62_Muon_nTrackerLayers-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/F-Muon_dxy-0/cluster21_Muon_dxy-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/F-Muon_dxyErr-0/cluster14_Muon_dxyErr-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/F-Muon_eta-0/cluster25_Muon_eta-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/B-Muon_isGlobal-0/cluster81_Muon_isGlobal-0_pg0.page", \
	"/var/scratch/alvarban/CERN_data/muonData/B-Muon_mediumId-0/cluster20_Muon_mediumId-0_pg0.page"};

#ifdef TIMING
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
#endif

#ifdef PARALLEL
  std::cout << "Running in parallel over " << N << " devices \n.";
  for(int s=0; s<N; s++) 
    run_benchmark_from_file(fnames[s], *myManager[s], verbose_memory, stream[s], num_iters);
#endif

#ifdef SERIAL
  std::cout << "Running in serially  over " << N << " devices \n.";
  for(int s=0; s<N; s++)
    run_benchmark_from_file(fnames[s], *myManager[0], verbose_memory, stream[0], num_iters);
#endif

#ifdef TIMING
  end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end - start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "----------\n finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
#endif 


  for (int s=0; s<N; s++)
  CUDA_CHECK(cudaStreamDestroy(stream[s]));

  return 0;
}
