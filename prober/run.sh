#!/bin/bash
export $(cat ../.env | xargs)
make
./probe