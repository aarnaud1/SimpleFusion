#!/usr/bin/sh

output_dir=output_OPC
dataset=../data/rgbd_dataset_freiburg1_xyz

mkdir -p ${output_dir}

# Run with different configurations
bin/main --dataset ${dataset} \
  --voxelRes 0.005 --tau 0.015 --maxDist 2.0 --useOPC \
  --outputFile ${output_dir}/output_fr3_0.005_0.015_2.0.ply

bin/main --dataset ${dataset} \
  --voxelRes 0.005 --tau 0.015 --maxDist 2.0 --useOPC \
  --outputFile ${output_dir}/output_fr3_0.01_0.015_2.0.ply

  bin/main --dataset ${dataset} \
  --voxelRes 0.005 --tau 0.02 --maxDist 2.0 --useOPC \
  --outputFile ${output_dir}/output_fr3_0.005_0.02_2.0.ply

  bin/main --dataset ${dataset} \
  --voxelRes 0.01 --tau 0.02 --maxDist 2.0 --useOPC \
  --outputFile ${output_dir}/output_fr3_0.01_0.02_2.0.ply

  bin/main --dataset ${dataset} \
  --voxelRes 0.005 --tau 0.03 --maxDist 2.0 --useOPC \
  --outputFile ${output_dir}/output_fr3_0.005_0.03_2.0.ply

  bin/main --dataset ${dataset} \
  --voxelRes 0.01 --tau 0.03 --maxDist 2.0 --useOPC \
  --outputFile ${output_dir}/output_fr3_0.01_0.03_2.0.ply
