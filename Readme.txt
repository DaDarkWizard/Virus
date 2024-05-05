I couldn't get the seed to be generated in the makefile (printf kept appending 16 bytes instead of 4), but it worked from command line, so the commands are as follows:

make
cp virus seed
printf '\xde\xad\xbe\xef' >> seed
cat host >> seed


