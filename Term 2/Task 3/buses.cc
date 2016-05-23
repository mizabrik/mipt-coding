#include <climits>

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

struct Time {
  int hour;
  int minute;
};

bool operator <=(const Time &first, const Time &second) {
  return first.hour < second.hour
         || (first.hour == second.hour && first.minute <= second.minute);
}

unsigned operator -(const Time &first, const Time &second) {
  int delta = (first.hour - second.hour) * 60 + first.minute - second.minute;
  return delta >= 0 ? delta : delta + 24 * 60;
}

Time operator +(const Time &time, unsigned delta) {
  Time updated = time;
  updated.hour = ((delta + time.minute) / 60) % 24;
  updated.minute = time.minute + delta % 60;
  return updated;
}

struct Route {
  int destination;

  Time departure;
  Time arrival;
};

struct City {
  std::vector<Route> routes;
};

std::vector<int> GetRoute(std::vector<City> &cities, int origin,
                          int destination, Time departure);

int main() {
  int cities_number;;
  std::cin >> cities_number;
  std::vector<City> cities(cities_number);

  int routes_number;
  std::cin >> routes_number;
  for (int i = 0; i < routes_number; ++i) {
    int origin;
    std::cin >> origin;
    cities[origin].routes.emplace_back();
    Route &route = cities[origin].routes.back();
    std::cin >> route.destination
        >> route.departure.hour >> route.departure.minute
        >> route.arrival.hour >> route.arrival.minute;
  }


  int origin, destination;
  Time departure;
  std::cin >> origin >> destination >> departure.hour >> departure.minute;

  auto route = GetRoute(cities, origin, destination, departure);

  if (!route.empty()) {
    for (auto city : route)
      std::cout << city << ' ';
    std::cout << std::endl;
  } else {
    std::cout << "NO WAY!!!" << std::endl;
  }
}

std::vector<int> GetRoute(std::vector<City> &cities, int origin,
                          int destination, Time departure) {
  struct Access {
    Time time;
    int length;
    int origin;
  };

  std::vector<Access> accessible(cities.size(), Access{Time{0, 0}, INT_MAX, -1});
  accessible[origin].length = 0;
  accessible[origin].time = departure;
  accessible[origin].origin = origin;

  auto compare = [&accessible] (int a, int b) {
    return accessible[a].length > accessible[b].length;
  };

  std::vector<int> queue;
  for (unsigned i = 0; i < cities.size(); ++i)
    queue.push_back(i);
  make_heap(queue.begin(), queue.end(), compare);

  while (!queue.empty()) {
    std::pop_heap(queue.begin(), queue.end(), compare);
    int city = queue.back();
    queue.pop_back();

    if (accessible[city].origin == -1)
      continue;
    for (auto &route : cities[city].routes) {
      int delta = (route.departure - accessible[city].time) + (route.arrival - route.departure);
      int new_length = accessible[city].length + delta;
      if (new_length < accessible[route.destination].length) {
        accessible[route.destination].length = new_length;
        accessible[route.destination].time = route.arrival;
        accessible[route.destination].origin = city;
        make_heap(queue.begin(), queue.end(), compare);
      }
    }
  }

  std::vector<int> answer;
  if (accessible[destination].origin != -1) {
    int city = destination;
    while(city != origin) {
      answer.push_back(city);
      city = accessible[city].origin;
    }
    answer.push_back(city);
    std::reverse(answer.begin(), answer.end());
  }

  return answer;
}
