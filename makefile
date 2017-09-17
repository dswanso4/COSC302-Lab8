all: city_map

clean:
	rm -f a.out core city_map city_map.o city_map_base.o

city_map: city_map.o city_map_base.o
	g++ -o city_map city_map.o city_map_base.o

city_map.o: city_map.cpp city_map.h
	g++ -c city_map.cpp

city_map_base.o: city_map.cpp city_map.h
	g++ -c city_map_base.cpp

