#!/usr/bin/env bash
HELP_USAGE="usage: $(basename "$0")  [-h] help
                        [-r] Rebase to master
                        [-c] Only run c specific tests
                        [-p] Only run python specific tests"
FAIL_OCCURED=0
LANG="c,py"

function run_test() {
    printf '============================================\n'
    printf '%-40s' "$1"
    if (($# == 1)); then
        printf 'SKIP\n'
        return
    fi

    CMD_OUTPUT=$(eval $2 2>&1)
    ECODE=$?
    if (($ECODE == 0)); then
        printf '\033[0;32mPASS\e[m\n> %s\n' "$2"
    else
        printf '\033[0;31mFAIL\e[m\n> %s\n' "$2"
        FAIL_OCCURED=1
    fi
    printf "%s\n" "$CMD_OUTPUT"
}

PHILIP_BASE="$( cd "$( dirname "${BASH_SOURCE[1]}" )" >/dev/null 2>&1 && pwd )"
cd $PHILIP_BASE

while getopts "hrcp" opt; do
  case $opt in
    h) echo "$HELP_USAGE"
    exit ;;
    c) LANG="c" ;;
    p) LANG="py" ;;
    r) REBASE=1 ;;
    *) echo 'error' >&2
       exit 1
  esac
done

if [ -n "$REBASE" ]; then
    run_test "REBASE" "git rebase master"
else
    run_test "REBASE"
fi

if [[ $LANG == "c,py" ]] || [[ $LANG == "py" ]]; then
  CMD_OUTPUT=$(flake8)
  run_test "FLAKE8" "flake8"
fi

run_test "CODESPELL" "codespell FW/Src/ FW/MMM/ FW/Inc IF/philip_pal/philip_pal QUALIFICATION/ --skip=\"*.csv,*.pyc,*stm32f1xx*\""

if [[ $LANG == "c,py" ]] || [[ $LANG == "c" ]]; then
  cd FW

  run_test "DOC_CHECK" "make doc"

  make clean
  CMD_OUTPUT=$(make)
  run_test "DEFAULT_MAKE" "make"
  make clean

  BOARD=BLUEPILL make clean
  CMD_OUTPUT=$(BOARD=BLUEPILL make)
  run_test "BLUEPILL_MAKE" "BOARD=BLUEPILL make"
  BOARD=BLUEPILL make clean

  BOARD=NUCLEOF103RB make clean
  run_test "NUCLEOF103RB_MAKE" "BOARD=NUCLEOF103RB make"
  BOARD=NUCLEOF103RB make clean
  cd ..
fi
