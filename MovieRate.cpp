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

// Function to load all user ratings from the dataset
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

// Function to calculate RatingDistance between two users
double calculateRatingDistance(const std::map<int, double> &user1Ratings, const std::map<int, double> &user2Ratings)
{
    double totalDistance = 0;
    int commonMovies = 0;

    // Start time point
    auto start_once = std::chrono::high_resolution_clock::now();

    // Iterate over the movies rated by user1
    for (const auto &[movieID, rating1] : user1Ratings)
    {
        // Check if user2 has rated the same movie
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
    std::cout << "Time taken by calculateRatingDistance() function: " << elapsed.count() << " seconds" << std::endl;
    
    }

    // If no common movies, return -1 to indicate no valid distance 
    if (commonMovies == 0)
    {
        return -1.0;
    }

    // Return the average distance
    return totalDistance / commonMovies;
}

// calculate the distance of every user to the target user and store in a heap
void calculateUserDistances(int targetUserID, const std::map<int, std::map<int, double>> &allUserRatings, std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(compareByValue)> &userDistances)
{
    const auto &targetUserRatings = allUserRatings.at(targetUserID);

    // Iterate over all users and record their distance to target user
    for (const auto &[userID, userRatings] : allUserRatings)
    {
        if (userID != targetUserID)
        {
            double distance = calculateRatingDistance(targetUserRatings, userRatings);

            // Insert user-distance pairs into the heap
            userDistances.push({userID, distance});
        }
    }

    return;
}

// Function to predict the rating for a movie using collaborative filtering
double predictRating(int targetUserID, int targetMovieID, const std::map<int, std::map<int, double>> &allUserRatings)
{
    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(compareByValue)> userDistances(compareByValue);
    calculateUserDistances(targetUserID, allUserRatings, userDistances);

    while (!userDistances.empty())
    {
        // Get the pair with the smallest value
        auto minUserDistance = userDistances.top();
        userDistances.pop();
        int minUser = minUserDistance.first;
        float minDistance = minUserDistance.second;
        // If a similar user is found and has rated the target movie
        if (minDistance!=-1 && allUserRatings.at(minUser).find(targetMovieID) != allUserRatings.at(minUser).end())
        {
            double predictedRating = allUserRatings.at(minUser).at(targetMovieID);
            std::cout << "The predicted rating for UserID: " << targetUserID << " and MovieID: " << targetMovieID
                      << " is " << predictedRating << " based on the most similar user: "
                      << minUser << " with a rating distance: " << minDistance << std::endl;
            return predictedRating;
        }
    }
    std::cerr << "Error: Could not find a similar user who has rated the target movie." << std::endl;
    return -1.0;
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

    // Parse command-line arguments
    int targetUserID = std::stoi(argv[1]);
    int targetMovieID = std::stoi(argv[2]);

    // Path to the MovieLens 10M ratings dataset
    std::string ratingsFile = "ratings.dat";

    // Load all user ratings from the dataset
    std::map<int, std::map<int, double>> allUserRatings = loadAllUserRatings(ratingsFile);

    // Predict the rating for the target movie
    double predictedRating = predictRating(targetUserID, targetMovieID, allUserRatings);

    // End time point
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> elapsed = end - start;

    // Output the time taken
    std::cout << "Time taken by main() function: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}