#!/bin/sh

gcc pre_process.c -o PreProcess

cp PreProcess zipf_data/

cd zipf_data

./PreProcess zipf_0.txt zipf_0.dat 256
./PreProcess zipf_1.txt zipf_1.dat 256
./PreProcess zipf_2.txt zipf_2.dat 256
./PreProcess zipf_3.txt zipf_3.dat 256
./PreProcess zipf_4.txt zipf_4.dat 256
./PreProcess zipf_5.txt zipf_5.dat 256
./PreProcess zipf_6.txt zipf_6.dat 256
./PreProcess zipf_7.txt zipf_7.dat 256
./PreProcess zipf_8.txt zipf_8.dat 256
./PreProcess zipf_9.txt zipf_9.dat 256
./PreProcess zipf_10.txt zipf_10.dat 256

rm PreProcess

cd ..

rm PreProcess

gcc process_flow.c -o ProcessFlow

mv ProcessFlow flow_data/

cd flow_data

./ProcessFlow 1 1.dat 256
./ProcessFlow 2 2.dat 256
./ProcessFlow 3 3.dat 256
./ProcessFlow 4 4.dat 256
./ProcessFlow 5 5.dat 256
./ProcessFlow 6 6.dat 256
./ProcessFlow 7 7.dat 256
./ProcessFlow 8 8.dat 256
./ProcessFlow 9 9.dat 256
./ProcessFlow 10 10.dat 256

rm ProcessFlow

cd ..


