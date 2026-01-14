#!/bin/bash

# Exit immediately on failure inside this script
set -e

target="none"
main_dir="$(pwd)"
run_evol=false

app_build_dir="${main_dir}/build"
tmp_dir="$(pwd)/tmp"

galib_build_dir="${main_dir}/galib247"
pmars_build_dir="${main_dir}/pmars-0.9.4/src"


build_evol_app()
{
    echo "======================================================="
    echo "Building Evolution App"
    sleep 1

    # make dirs if they don't exist
    mkdir -p "$app_build_dir"
    mkdir -p "$tmp_dir"

    cd "$app_build_dir" || { echo "ERROR: App build dir doesn't exist"; exit 1; }

    # call CMake to generate makefiles
    cmake .. 

    # build
    make
}

build_pMars()
{
    echo "======================================================="
    echo "Building pMars"
    sleep 1

    cd "$pmars_build_dir" || { echo "ERROR: pMars build dir doesn't exist"; exit 1; }

    make clean
    make

    if [ ! -f ./build/pmars ]; then
        echo "ERROR: pMars binary not found!"
        exit 1
    fi

    cp ./build/pmars "$main_dir"
}

build_galib()
{
    echo "======================================================="
    echo "Building galib"
    sleep 1

    cd "$galib_build_dir" || { echo "ERROR: galib build dir doesn't exist"; exit 1; }

    # Calls the previously fixed script
    ./build_galib_tester.sh
}


while getopts ":at:rh" opt; do
  case "$opt" in
    a) # (re)build all targets
      target="all"
      ;;
    t) # (re)build selected target only
      target="$OPTARG"
      ;;
    r) # run evolution
      run_evol=true
      ;;
    h|\?)
      echo "Usage: $(basename "$0")"
      echo "   [-a].......... (Re)build all targets."
      echo "   [-t arg].......(Re)build the selected target [pMars / galib / app]."
      echo "   [-r]...........Run Evolution App (everything has to be built before)."
      exit 0
      ;;
  esac
done
shift "$((OPTIND - 1))"


if [ "$target" = "all" ]; then
    build_pMars
    build_galib
    build_evol_app

elif [ "$target" = "pMars" ]; then
    build_pMars

elif [ "$target" = "galib" ]; then
    build_galib

elif [ "$target" = "app" ]; then
    build_evol_app

elif [ "$target" = "none" ]; then
    echo "No build target has been defined..."


else
    echo "Invalid target: $target" >&2
    exit 1
fi


if [ "$run_evol" = true ]; then
  cd "$app_build_dir" || exit 1

  echo "======================================================="
  echo "Running Evolution App (this can take longer)"
  sleep 0.5

  if ! ./corewar_ga; then
      echo "ERROR: Evolution App failed"
      exit 1
  fi
fi
