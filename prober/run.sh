#!/bin/bash
make
export $(cat .env | xargs)
./probe
