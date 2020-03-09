#define _CRT_SECURE_NO_WARNINGS

#include <mysql.h>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char email[100];
	char phone[50];
	char extension[10];
	char reportsTo[100];
	char jobTitle[50];
	char city[50];
};

int menu();

int findEmployee(MYSQL* conn, int employeeNumber, struct Employee* emp);

void displayEmployee(MYSQL* conn, struct Employee emp);

void displayAllEmployees(MYSQL* conn);

void insertEmployee(MYSQL* conn, struct Employee emp);

void updateEmployee(MYSQL* conn, int employeeNumber);

void deleteEmployee(MYSQL* conn, int employeeNumber);

int main(void) {

	MYSQL* conn;
	//intialize a storage Employee struct and it's respective data members
	Employee storage;

	storage.employeeNumber = 0;
	strcpy(storage.lastName, "\0");
	strcpy(storage.firstName, "\0");
	strcpy(storage.email, "\0");
	strcpy(storage.phone, "\0");
	strcpy(storage.extension, "\0");
	strcpy(storage.reportsTo, "\0");
	strcpy(storage.jobTitle, "\0");
	strcpy(storage.city, "\0");

	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "mymysql.senecacollege.ca", "db_mmaahi", "bWgg;%k@1L", "db_mmaahi", 3306, nullptr, 0);

	if (conn) {
		cout << "successful connection to database" << endl;
	}
	else {
		cout << "Connection Failed" << mysql_error(conn) << endl;
	}

	int select;
	int employeeNumber;

	do {

		select = menu();
		if (select == 1) {

			displayEmployee(conn, storage);

		}
		else if (select == 2) {

			displayAllEmployees(conn);

		}
		else if (select == 3) {

			insertEmployee(conn, storage);

		}
		else if (select == 4) {
			cout << "Employee Number: ";
			cin >> employeeNumber;
			updateEmployee(conn, employeeNumber);

		}
		else if (select == 5) {

			cout << "Employee Number: ";
			cin >> employeeNumber;
			deleteEmployee(conn, employeeNumber);

		}


	} while (select != 0);



	mysql_close(conn);
	return 0;
}

int menu() {

	int selection;

	cout << "********************* HR Menu *********************" << endl;
	cout << "1) Find Employee" << endl;
	cout << "2) Employees Report" << endl;
	cout << "3) Add Employee" << endl;
	cout << "4) Update Employee" << endl;
	cout << "5) Remove Employee" << endl;
	cout << "0) Exit" << endl;

	cin >> selection;
	cin.clear();
	if (selection < 0 || selection > 5) {

		while (selection < 0 || selection > 5) {

			cout << "Invalid Selection Please try again: ";
			cin >> selection;
			cin.clear();
		}
	}

	return selection;

}


int findEmployee(MYSQL* conn, int employeeNumber, struct Employee* emp) {

	int result = 0;
	string search_num = to_string(employeeNumber); //convert search criteria to a string
	string base_query = "SELECT e1.employeeNumber, e1.lastName, e1.firstName, "
		"e1.email, phone, e1.extension, "
		"CONCAT(e2.firstName,' ', e2.lastName) AS 'Manager ', e1.jobTitle, city "
		"FROM employees e1 LEFT JOIN employees e2 "
		"ON e1.reportsTo = e2.employeeNumber "
		"LEFT JOIN offices "
		"ON offices.officeCode = e1.officeCode "
		"WHERE e1.employeeNumber = ";
	string query = base_query + search_num;

	const char* q = query.c_str();
	int exequery;
	exequery = mysql_query(conn, q);
	if (exequery) {

		cout << "Error message: " << mysql_error(conn) << endl;
	}
	MYSQL_RES* res;
	MYSQL_ROW row;

	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);


	if (row == nullptr) { //Check if the search was successful
		cout << "The result is empty." << endl;

	}
	else {

		result = 1;
		emp->employeeNumber = stoi(row[0]);
		strcpy(emp->lastName, row[1]);
		strcpy(emp->firstName, row[2]);
		strcpy(emp->email, row[3]);
		strcpy(emp->phone, row[4]);
		strcpy(emp->extension, row[5]);
		if (row[6] == nullptr) {

			strcpy(emp->reportsTo, " ");
		}
		else {

			strcpy(emp->reportsTo, row[6]);

		}

		strcpy(emp->jobTitle, row[7]);
		strcpy(emp->city, row[8]);
	}
	return result;
}

void displayEmployee(MYSQL* conn, struct Employee emp) {

	int search_num;


	cout << "Please enter employee number: ";
	cin >> search_num;
	int success = findEmployee(conn, search_num, &emp);

	if (success == 1) {//Check if search was successful

		cout << "employeeNumber = " << emp.employeeNumber << endl;
		cout << "lastName = " << emp.lastName << endl;
		cout << "firstName = " << emp.firstName << endl;
		cout << "email = " << emp.email << endl;
		cout << "phone = " << emp.phone << endl;
		cout << "extension = " << emp.extension << endl;
		cout << "reportsTo = " << emp.reportsTo << endl;
		cout << "jobTitle = " << emp.jobTitle << endl;
		cout << "city = " << emp.city << endl;

	}
	else {

		cout << "Employee " << search_num << " does not exist" << endl;
	}
}


void displayAllEmployees(MYSQL* conn) {
	
	string query = "SELECT e1.employeeNumber, CONCAT(e1.firstName, ' ', e1.lastName) AS 'EmpName', "
		"e1.email, phone, e1.extension, "
		"CONCAT(e2.firstName,' ', e2.lastName) AS 'Manager '"
		"FROM employees e1 LEFT JOIN employees e2 ON e1.reportsTo = e2.employeeNumber LEFT JOIN offices ON offices.officeCode = e1.officeCode";
	const char* q = query.c_str();
	int exequery;
	exequery = mysql_query(conn, q);
	if (!exequery) {
		//query execution is successful
		cout << "The query executed successfully with no error." << endl;
	}
	else {
		//query execution is not successful
		cout << "Error message: " << mysql_error(conn) << endl;
	}

	MYSQL_RES* res;
	MYSQL_ROW row;

	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);
	if (row == nullptr) {
		cout << "The result is empty." << endl;
	}
	else {

		cout << "E         Employee Name          Email                              Phone               Ext      Manager" << endl;
		cout << "------------------------------------------------------------------------------------------------------------------------" << endl << endl;
	}
	cout.width(10); //space inbetween employee number and name
	cout.setf(ios::left);
	cout << row[0];

	cout.width(22);
	cout.setf(ios::left);
	cout << row[1] << " ";

	cout.width(35); //space inbetween employee number and name
	cout.setf(ios::left);
	cout << row[2];

	cout.width(20);
	cout << row[3];


	cout.width(10);
	cout << row[4];
	cout.width(9);
	
	cout << endl;
	while (row = mysql_fetch_row(res)) {

		
		cout.width(10); //space inbetween employee number and name
		cout.setf(ios::left);
		cout << row[0];
		
		cout.width(22);
		cout.setf(ios::left);
		cout << row[1] << " ";

		cout.width(35); //space inbetween employee number and name
		cout.setf(ios::left);
		cout << row[2];
		
		cout.width(20);
		cout << row[3];


		cout.width(10);
		cout << row[4];
		cout.width(9);
		cout << row[5];
		cout << endl;
	}


}

void updateEmployee(MYSQL* conn, int employeeNumber) {


	Employee storage;

	storage.employeeNumber = 0;
	strcpy(storage.lastName, "\0");
	strcpy(storage.firstName, "\0");
	strcpy(storage.email, "\0");
	strcpy(storage.phone, "\0");
	strcpy(storage.extension, "\0");
	strcpy(storage.reportsTo, "\0");
	strcpy(storage.jobTitle, "\0");
	strcpy(storage.city, "\0");

	
	int success = findEmployee(conn, employeeNumber, &storage);
	if (success == 1) {
		string ext;
		cout << "New Extension: ";
		cin >> ext;

		string base_statement = "UPDATE employees SET extension = '" + ext + "'";
		string number = to_string(employeeNumber);
		string second_statement = "WHERE employeeNumber = '" + number + "'";

		string query = base_statement + second_statement;

		const char* q = query.c_str();
		int exequery;
		exequery = mysql_query(conn, q);
		if (!exequery) {
			//query execution is successful
			cout << "The query executed successfully with no error." << endl;
		}
		else {
			//query execution is not successful
			cout << "Error message: " << mysql_error(conn) << endl;
		}

	}


}


void deleteEmployee(MYSQL* conn, int employeeNumber) {

	Employee storage;

	storage.employeeNumber = 0;
	strcpy(storage.lastName, "\0");
	strcpy(storage.firstName, "\0");
	strcpy(storage.email, "\0");
	strcpy(storage.phone, "\0");
	strcpy(storage.extension, "\0");
	strcpy(storage.reportsTo, "\0");
	strcpy(storage.jobTitle, "\0");
	strcpy(storage.city, "\0");


	int success = findEmployee(conn, employeeNumber, &storage);
	if (success == 1) {

		string base = "DELETE FROM employees WHERE employeeNumber = ";// base statement for the Delete
		string emp_number = to_string(employeeNumber);
		string delete_query = base + emp_number; // combine for the required query statement
		const char* q = delete_query.c_str();

		cout << delete_query << endl;
		int exequery;
		exequery = mysql_query(conn, q);
		if (!exequery) {
			//query execution is successful
			cout << "The employee is deleted." << endl;
		}
		else {
			//if the query was not successful it means that the employee number is a foreign key to be removed

			string set_null = "UPDATE customers SET salesRepEmployeeNumber = null WHERE salesRepEmployeeNumber = "; //Update the customers table that references emplo
			string rep_null = set_null + emp_number;

			const char* sales = rep_null.c_str();

			int check;
			check = mysql_query(conn, sales);

			if (!check) {
				//query execution is successful

				//once successful re-rerun the  original delete query again
				mysql_query(conn, q);
				cout << "The employee is deleted." << endl;

			}

		}

	}
	else {

		cout << "The employee does not exist." << endl;

	}
}

void insertEmployee(MYSQL* conn, struct Employee emp)
{
	Employee storage;

	storage.employeeNumber = 0;
	strcpy(storage.lastName, "\0");
	strcpy(storage.firstName, "\0");
	strcpy(storage.email, "\0");
	strcpy(storage.phone, "\0");
	strcpy(storage.extension, "\0");
	strcpy(storage.reportsTo, "\0");
	strcpy(storage.jobTitle, "\0");
	strcpy(storage.city, "\0");

	// Variables for taking user's infos
	int employeeNum;
	string lName, fName, mail, ext, jobT;
	string rpTo = "1002", ofcCode = "1";

	// Read Employee number first for checking existing number
	cout << "Employee Number: ";
	cin >> employeeNum;

	// Checking for Employee number's availability
	int success;
	success = findEmployee(conn, employeeNum, &storage);

	// If number isn't used yet, read new employee's infos
	if (success != 1)
	{
		// Change Employee number to string for data storing
		string empNumStr = to_string(employeeNum);
		// Read new employee's infos
		cout << "Last Name: ";
		cin >> lName;
		cout << "First Name: ";
		cin >> fName;
		cout << "Email: ";
		cin >> mail;
		cout << "extension: ";
		cin >> ext;
		cout << "Job Title: ";
		cin >> jobT;


		// INSERT STATEMENT with user's entered data
		string insert = "INSERT INTO employees (employeeNumber, lastName, firstName, email, extension, officeCode, reportsTo, jobTitle)"
			"VALUES ('" + empNumStr + "', '" + lName + "', '" + fName + "', '" + mail + "', '" + ext + "', '" + ofcCode + "', '" + rpTo + "', '" + jobT + "')";

		// Checking query's success
		int exequery;
		exequery = mysql_query(conn, insert.c_str());

		// Query successful. New employee added.
		if (!exequery)
		{
			cout << "The new employee is added successfully" << endl;
		}
		// Query unsuccessful. Display error messages
		else
		{
			cout << mysql_error(conn) << endl << endl;
		}
	}
	// Number is already used. Cancel adding.
	else
	{
		cout << "An employee with the same employee number exists." << endl;
	}
}





