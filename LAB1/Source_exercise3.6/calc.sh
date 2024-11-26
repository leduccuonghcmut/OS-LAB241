#!/bin/bash
history_file="answer.txt"
if [ -f "$history_file" ]; then
  read -r ANS < "$history_file"
else
  ANS=0
fi
declare -a hist=()
while true; do
  clear
  read -p ">> " var1 var2 var3

  if [ "$var1" == "ANS" ]; then
      var1=$ANS
  fi
  if [ "$var3" == "ANS" ]; then
      var3=$ANS
  fi
  if [ "$var1" == "EXIT" ]; then
      break
  elif [ "$var1" == "HIST" ]; then
      for ((i=0; i<5 && i<${#hist[@]}; i++)); do
          echo "${hist[$i]}"
      done
      read -n 1 -s -r -p ""
      continue
  fi
  

  # Kiem tra tinh hop le cua cac bien nhap vao
  if ! [[ "$var1" =~ ^[+-]?[0-9]+([.][0-9]+)?$ ]] || ! [[ "$var3" =~ ^[+-]?[0-9]+([.][0-9]+)?$ ]]; then
    echo "MATH ERROR"
    read -n 1 -s -r -p ""
    continue
  fi


  # Kiem tra chia cho 0 trong phep chia hoac chia lay du
  if [[ "$var3" == "0" && ( "$var2" == "/" || "$var2" == "%" ) ]]; then
    echo "MATH ERROR"
    read -n 1 -s -r -p ""
    continue
  fi

  # Kiem tra phep toan hop le
  if [ "$var2" != "+" ] && [ "$var2" != "-" ] && [ "$var2" != "*" ] && [ "$var2" != "/" ] && [ "$var2" != "%" ]; then
      echo "SYNTAX ERROR"
      read -n 1 -s -r -p ""
      continue
  fi

  # Tinh toan ket qua
  case $var2 in
      +) result=$(echo "$var1 + $var3" | bc)
      ;;
      -) result=$(echo "$var1 - $var3" | bc)
      ;;
      \*) result=$(echo "$var1 * $var3" | bc)
      ;;
      %) result=$(echo "$var1 % $var3" | bc)
      ;;
      /) 
          result=$(echo "scale=3; $var1 / $var3" | bc)  # Tinh truoc voi 3 chu so thap phan
          result=$(printf "%.2f" "$result")             # Lam tron len 2 chu so thap phan
      ;;
  esac
  
  # Cap nhat va luu ket qua
  hist+=("$var1 $var2 $var3 = $result")
  ANS=$result 
  echo "$ANS" > "$history_file"
  echo "$result" 
  read -n 1 -s -r -p ""
done

