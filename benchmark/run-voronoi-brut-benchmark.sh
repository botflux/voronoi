#!/bin/bash

LOG_FILE_NAME=brut.log
START_ROW_COUNT=10
START_COLUMN_COUNT=10
START_GERMS=$(($START_COLUMN_COUNT * $START_ROW_COUNT * 1 / 100))


ADD=10

GERMS=$START_GERMS
COLUMN_COUNT=$START_COLUMN_COUNT
ROW_COUNT=$START_ROW_COUNT

for i in {1..100}
do
  ./dist/VoronoiBrut --rowCount $ROW_COUNT --columnCount $COLUMN_COUNT --distanceType 0 --germs $GERMS --skipImage >> $LOG_FILE_NAME
  ./dist/VoronoiBrut --rowCount $ROW_COUNT --columnCount $COLUMN_COUNT --distanceType 1 --germs $GERMS --skipImage >> $LOG_FILE_NAME
  ./dist/VoronoiBrut --rowCount $ROW_COUNT --columnCount $COLUMN_COUNT --distanceType 2 --germs $GERMS --skipImage >> $LOG_FILE_NAME

  ROW_COUNT=$(($ROW_COUNT + $ADD))
  COLUMN_COUNT=$(($COLUMN_COUNT + $ADD))
  GERMS=$(($COLUMN_COUNT * $ROW_COUNT * 1 / 100))
done