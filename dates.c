#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to get the current date
date getCurrentDate() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    date current_date;
    current_date.day = tm->tm_mday;
    current_date.month = tm->tm_mon + 1;
    current_date.year = tm->tm_year + 1900;
    return current_date;
}

// Function to calculate the number of days in a given month of a given year
int daysInMonth(int month, int year) {
    int days[] = {31, 28 + (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month - 1];
}

// Function to calculate the number of days since January 1, 1900 for a given date
int daysSince1900(date d) {
    int days = 0;
    int year;

    // Add days for previous years
    for (year = 1900; year < d.year; year++) {
        days += 365 + (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    }

    // Add days for previous months in the current year
    for (int month = 1; month < d.month; month++) {
        days += daysInMonth(month, d.year);
    }

    // Add days for the current month
    days += d.day;

    return days;
}

// Function to calculate the number of days between two dates
int daysBetweenDates(date d1, date d2) {
    int days1 = daysSince1900(d1);
    int days2 = daysSince1900(d2);
    return days2 - days1;
}

// Function to calculate the days left between today and a given date
int daysLeftToDate(date target_date) {
    date current_date = getCurrentDate();
    return daysBetweenDates(current_date,target_date);
}