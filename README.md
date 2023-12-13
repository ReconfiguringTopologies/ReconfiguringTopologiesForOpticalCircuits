# Reconfiguring Topologies For Optical Circuits
In the rapidly evolving field of large models in machine learning, it has become increasingly important to maximize the throughput of model training. Thus, optical circuit-based supercomputers like TPU v4 offer a unique opportunity to reconfigure network topologies based on the training task being run. An important family of topologies for such applications are twisted tori, which have been shown to significantly improve throughput for all-to-all communication patterns. Over 25% of workloads run on TPU v4 utilize twisted tori topologies. However, prior works only evaluate a subset of twisted tori topologies, and do not consider communication patterns seen in modern machine learning training workflows. In this paper, we use a network simulator to systematically evaluate potential twisting patterns for various communication patterns and parallelism schemes, and provide conceptual and mathematical justification for why certain patterns in communication time are observed. Our results demonstrate that twisted tori topologies offer improvements of up to 40% in average communication delay for model-tensor parallelism communication patterns, and identify the optimal twists for a number of common network dimensions.

In order to run the files, you should use the ns3 exec file by 
``` ./ns3 run scratch/<filename> ```
The CSV file will contain all of the information about the desired metrics, this can be later formatted with the ```parsing.py``` file. 
The heatmap version will also need to be executed with ns3 and then formatted with ```heatmap_script.py```
