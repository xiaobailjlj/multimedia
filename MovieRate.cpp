#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <queue>

// Define a custom comparator for the priority queue (min-heap) based on value
auto compareByValue = [](const std::pair<int, double> &a, const std::pair<int, double> &b)
{
    return a.second > b.second; // Compare based on the value (second element)
};

std::map<int, std::map<int, double>> loadAllUserRatings(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    std::map<int, std::map<int, double>> userRatings; // userID -> {movieID -> rating}
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string userID_str, movieID_str, rating_str, timestamp;

        // Extract data from the line
        std::getline(ss, userID_str, ':');
        ss.ignore(1); // Ignore extra ':'
        std::getline(ss, movieID_str, ':');
        ss.ignore(1); // Ignore extra ':'
        std::getline(ss, rating_str, ':');
        ss.ignore(1); // Ignore extra ':'
        std::getline(ss, timestamp);

        int userID = std::stoi(userID_str);
        int movieID = std::stoi(movieID_str);
        double rating = std::stod(rating_str);

        // Store the rating
        userRatings[userID][movieID] = rating;
    }

    return userRatings;
}

double calculateRatingDistance(const std::map<int, double> &user1Ratings, const std::map<int, double> &user2Ratings)
{
    double totalDistance = 0;
    int commonMovies = 0;

    // Start time point
    auto start_once = std::chrono::high_resolution_clock::now();

    for (const auto &[movieID, rating1] : user1Ratings)
    {
        // add distance only if both users have seen this movie
        if (user2Ratings.find(movieID) != user2Ratings.end())
        {
            double rating2 = user2Ratings.at(movieID);         
            totalDistance += std::abs(rating1 - rating2);
            commonMovies++;
        }
    // End time point
    auto end_once = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> elapsed = end_once - start_once;

    // Output the time taken
    // std::cout << "Time taken by calculateRatingDistance() function: " << elapsed.count() << " seconds" << std::endl;
    
    }

    // no common movies
    if (commonMovies == 0)
    {
        return -1.0;
    }

    return totalDistance / commonMovies;
}

// calculate the distance of every user to the target user and store in a heap
void calculateUserDistances(int targetUserID, const std::map<int, std::map<int, double>> &allUserRatings, std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(compareByValue)> &userDistances)
{
    const auto &targetUserRatings = allUserRatings.at(targetUserID);

    for (const auto &[userID, userRatings] : allUserRatings)
    {
        if (userID != targetUserID)
        {
            double distance = calculateRatingDistance(targetUserRatings, userRatings);
            userDistances.push({userID, distance});
        }
    }

    return;
}

// main logic to calculate most similar user and predict rate
void MovieRate(int targetUserID, int targetMovieID, const std::map<int, std::map<int, double>> &allUserRatings, int &simialrUser, double &predictRating, double&minDistance)
{
    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(compareByValue)> userDistances(compareByValue);
    calculateUserDistances(targetUserID, allUserRatings, userDistances);

    while (!userDistances.empty())
    {
        // Get the user with min diatance
        auto minUserDistance = userDistances.top();
        userDistances.pop();
        int minUser = minUserDistance.first;
        minDistance = minUserDistance.second;
        // If a similar user is found and has rated the target movie
        if (minDistance!=-1 && allUserRatings.at(minUser).find(targetMovieID) != allUserRatings.at(minUser).end())
        {
            predictRating = allUserRatings.at(minUser).at(targetMovieID);
            simialrUser = minUser;
            return;
        }
    }
    std::cerr << "unable to find a similar user who has rated the target movie." << std::endl;
    return;
}

// ./MovieRate 30801 1407
int main(int argc, char *argv[])
{
    // Start time point
    auto start = std::chrono::high_resolution_clock::now();
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " [UserID] [MovieID]" << std::endl;
        return 1;
    }

    int targetUserID = std::stoi(argv[1]);
    int targetMovieID = std::stoi(argv[2]);

    std::string ratingsFile = "ratings.dat";

    // Load all user ratings
    std::map<int, std::map<int, double>> allUserRatings = loadAllUserRatings(ratingsFile);

    int simialrUser = -1;
    double predictRating = -1;
    double minDistance = -1;

    MovieRate(targetUserID, targetMovieID, allUserRatings, simialrUser, predictRating, minDistance);

    std::cout << "The predicted rating for UserID: " << targetUserID << " and MovieID: " << targetMovieID
                      << " is " << predictRating << " based on the most similar user: "
                      << simialrUser << " with a rating distance: " << minDistance << std::endl;

    // End time point
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> elapsed = end - start;

    // Output the time taken
    std::cout << "Time taken by main() function: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}