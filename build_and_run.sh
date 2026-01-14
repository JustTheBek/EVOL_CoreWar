#!/bin/bash

# Exit immediately on failure inside this script
set -e

target="none"
main_dir="$(pwd)"
run_evol=false

# Default GA parameters
ga_population=20
ga_generations=100
ga_mutation=0.05
ga_crossover=0.9

app_build_dir="${main_dir}/build"
tmp_dir="$(pwd)/tmp"

galib_build_dir="${main_dir}/galib247"
pmars_build_dir="${main_dir}/pmars-0.9.4/src"


build_evol_app()
{
    echo "======================================================="
    echo "Building Evolution App"
    sleep 1

    # make tmp dir if it doesn't exist
    mkdir -p "$tmp_dir"

    # go to main dir for safety
    cd "$main_dir" || { echo "ERROR: Main directory not found"; exit 1; }

    # fully clean build folder
    rm -rf "$app_build_dir"
    mkdir -p "$app_build_dir"
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


# Parse command-line options
while getopts ":at:rhp:g:m:c:" opt; do
  case "$opt" in
    a) target="all" ;;
    t) target="$OPTARG" ;;
    r) run_evol=true ;;
    p) ga_population="$OPTARG" ;;
    g) ga_generations="$OPTARG" ;;
    m) ga_mutation="$OPTARG" ;;
    c) ga_crossover="$OPTARG" ;;
    h|\?) 
      echo "Usage: $(basename "$0")"
      echo "   [-a].............. (Re)build all targets."
      echo "   [-t <target>]..... (Re)build the selected target [pMars / galib / app]."
      echo "   [-r].............. Run Evolution App (requires build first)."
      echo "   [-p <population>].. GA population size (default: 20)"
      echo "   [-g <generations>]  GA number of generations (default: 100)"
      echo "   [-m <mutation>].... GA mutation probability (default: 0.05)"
      echo "   [-c <crossover>]... GA crossover probability (default: 0.9)"
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

  if ! ./corewar_ga "$ga_population" "$ga_generations" "$ga_mutation" "$ga_crossover"; then
    echo "ERROR: Evolution App failed"
    exit 1
  fi
fi
