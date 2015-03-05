#!/bin/sh
cd `dirname $0` &&
g++ -o test_xml_parser ../xml_parser.cc main.cc -std=c++0x -I../.. -lstdc++ -rdynamic -g `xml2-config --cflags --libs` &&
./test_xml_parser test-manifest.xml &&
which valgrind && valgrind --tool=memcheck --leak-check=full ./test_xml_parser test-manifest.xml

rm -f test_xml_parser
