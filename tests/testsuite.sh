#!/bin/bash --posix
# configure_input


green="\033[38;5;82m"
red="\033[38;5;217m"
strongred="\033[1;38;5;196m"
default="\033[0m"
bold="\033[1m"

TC=./src/tc

if [ ! -x "$TC" ]; then
  echo -e "${red}Error:${default} compiler not found at $TC"
  exit 1
fi

echo
echo -e "${bold}Running tests from: tests/good/${default}"
echo

total=0
passed=0
passed_list=""
compile_failed=""
pretty_failed=""

for file in tests/good/*.tig; do
  testname=$(basename "$file" .tig)
  total=$((total + 1))

  stage1_out=$(mktemp)
  stage2_out=$(mktemp)

 
  if ! "$TC" -A "$file" > "$stage1_out" 2>&1 || grep -qi "syntax error" "$stage1_out"; then
    compile_failed="${compile_failed}${bold}${red}${testname}${default}\n$(cat "$stage1_out")\n===\n"
    rm -f "$stage1_out" "$stage2_out"
    continue
  fi

  
  if ! "$TC" -A - < "$stage1_out" > "$stage2_out" 2>&1 || grep -qi "syntax error" "$stage2_out"; then
    pretty_failed="${pretty_failed}${bold}${red}${testname}${default}\n$(cat "$stage2_out")\n===\n"
    rm -f "$stage1_out" "$stage2_out"
    continue
  fi

 
  echo -e "${green}${testname} passed${default}"
  passed_list="${passed_list}${bold}${testname}${default}\n"
  passed=$((passed + 1))

  rm -f "$stage1_out" "$stage2_out"
done


echo
echo -e "${bold}Finished running tests from tests/good/.${default}"
echo -e "${bold}Success:${default} ${green}${passed}${default}/${total} tests"


if [ "$passed" -gt 0 ]; then
  echo
  echo -e "${green}Passed tests:${default}"
  printf "%b" "$passed_list"
fi


if [ -n "$compile_failed" ]; then
  echo
  echo -e "${strongred}COMPILE failures:${default}"
  echo -e "${red}-------------------------------------${default}"
  printf "%b" "$compile_failed" | while IFS= read -r line; do
    [ "$line" = "===" ] && echo -e "\n${red}-------------------------------------${default}" && continue
    echo "  $line"
  done
fi


if [ -n "$pretty_failed" ]; then
  echo
  echo -e "${strongred}PRETTY-PRINT failures:${default}"
  echo -e "${red}-------------------------------------${default}"
  printf "%b" "$pretty_failed" | while IFS= read -r line; do
    [ "$line" = "===" ] && echo -e "\n${red}-------------------------------------${default}" && continue
    echo "  $line"
  done
fi


