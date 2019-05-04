#! /bin/bash

echo "run python tests..."
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

#
# read the list of all python test scripts
#
TEST_SCRIPTS=`find . -name \*.py | grep -v Python`

if [ "$#" -eq 1 ]; then
    TEST_SCRIPTS=$1
fi

echo "==============================================================="
echo "==============================================================="
echo ""


TEST_SCRIPT_FAILED=0
TEST_SCRIPT_SUCCESS=0
RESULTS=()

#
# ececute all test scripts
#

export PYTHONPATH=Python

for i in ${TEST_SCRIPTS[@]}; do
    SCRIPT=$i

    echo "run test script ${SCRIPT} ..."
    python3 ${SCRIPT}

    if [ $? -ne 0 ]; then
        echo -e "${RED}test script ${SCRIPT}  failed${NC}"
        TEST_SCRIPT_FAILED=$(( ${TEST_SCRIPT_FAILED} + 1 ))
        RESULTS+=('FAILED')
    else
        echo -e "${GREEN}test script ${SCRIPT} success${NC}"
        TEST_SCRIPT_SUCCESS=$(( ${TEST_SCRIPT_SUCCESS} + 1 ))
        RESULTS+=('SUCCESS')
    fi

done

#
# display statistic
#
echo "==============================================================="
echo "==============================================================="
echo ""
echo "Test Script Statistic:"
Index=0
for i in ${TEST_SCRIPTS[@]}; do
    if [ ${RESULTS[$INDEX]} == "SUCCESS" ]; then
        echo -e "${GREEN}$i success${NC}" 
    else
        echo -e "${RED}$i failed${NC}"
    fi
    ((INDEX++))
done
echo ""
echo "Success: ${TEST_SCRIPT_SUCCESS}"
echo "Failed:  ${TEST_SCRIPT_FAILED}"
echo ""

if [ ${TEST_SCRIPT_FAILED} -eq 0 ]; then
    echo -e "${GREEN}TEST SUCCESS${NC}"
    exit 0
else
    echo -e "${RED}TEST FAILED${NC}"
    exit 1
fi
