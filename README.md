# :page_facing_up: NVCOMP_CompressionBenchmarks
This repository is used as data storage for all data gathered throuhgot my final university project. The research project performed in 2022 at the University of Twente in the Netherlands, follows to develop an analytical model that can be used to estimate the performance of compressors on the GPU. Moreover, the main research question analyzes the difference between the CPU-based decompression and the possible improvements followed by GPU decompression


The full thesis can be read by accessing the following URL: [Enabling faster processing of big-data through GPU decompression](https://www.google.com)

# :computer: Testing setup
The table below lists the hardware used to gather data. Furthermore, details about the DAS-6 system can be found here [DAS-6](https://www.cs.vu.nl/das/) :like:

|       | UvA Showcees         | DAS-6            |
|-------|----------------------|------------------|
| CPU   | Intel Xeon Gold 6148 | AMD EPYC-2 7402P |
|       |                      | AMD EPYC-2 7282  |
| GPU*  | NVIDIA GTX 1080Ti    | NVIDIA RTX A4000 |
|       |                      | NVIDIA RTX A5000 |
|       |                      | NVIDIA RTX A6000 |
| GPU   |                      | NVIDIA A100      |
| RAM** | 400GB                | 128 GB           |
| OS    | Cent OS 7            | Rocky Linux 8    |

# Quick results:

### Best Algorithms using the Analytical Model
| Data    | Compression | Decompression | Ratio    |
|---------|-------------|---------------|----------|
| Bool    | ANS         | ZLIB          | FastLZMA |
| Float   | Cascaded    | Cascaded      | FastLZMA |
| Integer | Bitcomp     | Cascaded      | FastLZMA |
| String  | Bitcomp     | ANS           | ANS      |

### Compression Ratio 
![Compression Ratio results](/Charts/General_CR_chart.png)

### Compression Throughput (GB/s)
![Compression Throughput](/Charts/General_CT_chart.png)

### Decompression Throughput (GB/s)
![Decompression Throughput](/Charts/General_DT_chart.png)

Furthermore, our analysis has analyzed and indirectly proved that chunk sizes have a high impact on the performance of GPU compression/decompression. Our results show that the lower the chunk size, the higher the throughput while the ratio usually takes a hit.

![Chunk size performance](/Charts/Chunk_Chart.png)