#/usr/bin/env bash
set -eu

commands=(
  ################################################################################
  # mnist HCNN (Thread: 1, 72)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 5"
  # "OMP_NUM_THREADS=1 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D mnist -M HCNN-DA -N 3"

  ################################################################################
  # mnist CKKS-swish -O4 (Thread: 1, 72)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 7"
  # "OMP_NUM_THREADS=1 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=1 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"

  ################################################################################
  # mnist CKKS-swish (Thread: 1, 72)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 11"
  # "OMP_NUM_THREADS=1 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=1 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"


  ################################################################################
  # cifar-10 HCNN (Thread: 72, 32)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 8"
  # "OMP_NUM_THREADS=72 ../bin/main -D cifar-10 -M HCNN-DA -O 3 -N 2"
  # "OMP_NUM_THREADS=32 ../bin/main -D cifar-10 -M HCNN-DA -N 3"

  "OMP_NUM_THREADS=1 ../bin/setup -P 14 -L 8"
  "OMP_NUM_THREADS=72 /usr/bin/time -v ../bin/main -D cifar-10 -M HCNN-DA -O 3 -N 2"

  ################################################################################
  # cifar-10 CKKS-swish -O4 (Thread: 72, 32)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 11"
  # "OMP_NUM_THREADS=72 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 2"
  # "OMP_NUM_THREADS=32 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 2"
  # "OMP_NUM_THREADS=32 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"

  "OMP_NUM_THREADS=1 ../bin/setup -P 14 -L 11"
  "OMP_NUM_THREADS=72 /usr/bin/time -v ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 2"
  "OMP_NUM_THREADS=72 /usr/bin/time -v ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 2"

  ################################################################################
  # cifar-10 CKKS-swish (Thread: 72, 32)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 20"
  # "OMP_NUM_THREADS=72 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=32 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=72 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=32 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"


  ################################################################################
  # mnist HCNN (Thread: 32, 18, 16, 8, 4, 2)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 5"
  # "OMP_NUM_THREADS=32 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=12 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D mnist -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D mnist -M HCNN-DA -N 3"

  "OMP_NUM_THREADS=1 ../bin/setup -P 14 -L 5"
  "OMP_NUM_THREADS=16 /usr/bin/time -v numactl -N 3 ../bin/main -D mnist -M HCNN-DA -N 2"

  ################################################################################
  # mnist CKKS-swish -O4 (Thread: 32, 18, 16, 8, 4, 2)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 7"
  # "OMP_NUM_THREADS=32 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=12 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=32 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=12 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"

  "OMP_NUM_THREADS=1 ../bin/setup -P 14 -L 7"
  "OMP_NUM_THREADS=16 /usr/bin/time -v numactl -N 3 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 2"
  "OMP_NUM_THREADS=16 /usr/bin/time -v numactl -N 3 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 2"

  ################################################################################
  # mnist CKKS-swish (Thread: 32, 18, 16, 8, 4, 2)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 11"
  # "OMP_NUM_THREADS=32 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=12 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D mnist -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=32 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=12 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D mnist -M CKKS-swish_rg6_deg4-BN-DA -N 3"


  ################################################################################
  # cifar-10 HCNN (Thread: 1, 18, 16, 8, 4, 2)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 11"
  # "OMP_NUM_THREADS=1 ../bin/main -D cifar-10 -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D cifar-10 -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D cifar-10 -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D cifar-10 -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D cifar-10 -M HCNN-DA -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D cifar-10 -M HCNN-DA -N 3"

  ################################################################################
  # cifar-10 CKKS-swish -O4 (Thread: 32, 18, 16, 8, 4, 2)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 14"
  # "OMP_NUM_THREADS=1 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=1 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -O 4 -N 3"

  ################################################################################
  # cifar-10 CKKS-swish (Thread: 32, 18, 16, 8, 4, 2)
  ################################################################################
  # "OMP_NUM_THREADS=1 ../bin/setup -P 15 -L 20"
  # "OMP_NUM_THREADS=1 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D cifar-10 -M CKKS-swish_rg4_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=1 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=18 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=16 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=8 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=4 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
  # "OMP_NUM_THREADS=2 ../bin/main -D cifar-10 -M CKKS-swish_rg6_deg4-BN-DA -N 3"
)

if [ $# -ne 1 ]; then
  echo "Please specify log file name"
  echo "usage: ./execute.sh ${LOG_FILE_NAME}"
  exit 1
fi

log_path="../logs/"
log_file_name=$1
# log_file_path="${log_path}main_log.txt"
log_file_path="${log_path}${log_file_name}"

[[ -d "${log_path}" ]] || mkdir "${log_path}"
[[ -f "${log_file_path}" ]] || touch "${log_file_path}"

for command in "${commands[@]}"
do
  /bin/echo -n "[$(date)]" >> "${log_file_path}"
  echo ": \"${command}\" started" >> "${log_file_path}"
  eval "${command}" >> "${log_file_path}"
  /bin/echo -n "[$(date)]" >> "${log_file_path}"
  echo ": \"${command}\" finished" >> "${log_file_path}"
  echo -e "\n" >> "${log_file_path}"
done
