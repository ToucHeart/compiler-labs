echo "--------------------------------" > ./Test/output
for i in ./Tests/*.cmm
do 
    echo $i >> ./Test/output;
    ./Code/parser $i >> ./Test/output;
    if [ $? -ne 0 ]; 
    then
        echo $i;
    fi
    echo "------------------------------" >> ./Test/output
done