
#include <iostream>

#include "PerformanceAnalyzer.h"
#include "DataAnalyzer.h"
#include <iostream>

using namespace std;
using namespace Smokin;

#ifdef _WIN32
#include <windows.h>
#else
	#include <unistd.h> // usleep?
#endif //_WIN32

void main()
{
	assert( PerformanceAnalyzer::numberOfQueries() == 0);
	assert( !PerformanceAnalyzer::hasQuery("Simple Query #1") );

	PerformanceAnalyzer::Query query("Simple Query #1");
	for (unsigned int i = 0; i != 15; ++i)
	{
		query.start();
		cout << "Simple Query Test" << endl;
		query.end();
	}
	assert( PerformanceAnalyzer::numberOfQueries() == 1);
	assert( PerformanceAnalyzer::hasQuery("Simple Query #1") );


	for (unsigned int i = 0; i != 10; ++i)
	{
		Smokin::PerformanceAnalyzer::ScopedQuery query("Scoped Query #1");
		cout << "Scoped Query test" << i * i << endl;
	}
	assert( PerformanceAnalyzer::numberOfQueries() == 2);
	assert( PerformanceAnalyzer::hasQuery("Scoped Query #1") );


	//"Scoped Query #2"
	{
		float one = 1;
		float another = 1;
		Smokin::PerformanceAnalyzer::ScopedQuery query("Scoped Query #2");
		for (unsigned int i = 0; i != 100; ++i)
		{
			one *= one;
			another *= another;
			
			one += i;
			another *= one;
		}
	}


	int sleep_time = 100;
	for (unsigned int i = 0; i != 100; ++i)
	{
		Smokin::PerformanceAnalyzer::ScopedQuery query("Sleep Test");

#ifdef _WIN32
		Sleep( sleep_time );
#else
		usleep( sleep_time * 1000);
#endif //_WIN32

	}

	for (unsigned int i = 0; i != 10; ++i)
	{
		Smokin::PerformanceAnalyzer::DelayedQuery delayed_query("Delayed Query", 3000.0, 5000.0);
		delayed_query.start();
		cout << "Delayed Query" << endl;
		delayed_query.end();
	}

	{ 
		Smokin::PerformanceAnalyzer::ScopedDelayedQuery scoped_delayed_query("Scoped Delayed Query", 5000.0);
		cout << "Scoped Delayed Query" << endl;
	}


	Smokin::PerformanceAnalyzer::ToggleQuery toggle_query("Toggle Query");

	toggle_query.toggle();
	cout << "Toggle Delayed Query" << endl;
	toggle_query.toggle();
	cout << "Toggle Delayed Query" << endl;
	toggle_query.toggle();
	cout << "Toggle Delayed Query" << endl;
	toggle_query.toggle();


	Smokin::PerformanceAnalyzer::MarkerQuery("Marker Query");
	cout << "Marker Query " << endl;
	Smokin::PerformanceAnalyzer::MarkerQuery("Marker Query");
	cout << "Marker Query " << endl;
	Smokin::PerformanceAnalyzer::MarkerQuery("Marker Query");
	cout << "Marker Query " << endl;
	Smokin::PerformanceAnalyzer::MarkerQuery("Marker Query");
	cout << "Marker Query " << endl;
	Smokin::PerformanceAnalyzer::MarkerQuery("Marker Query");


	Smokin::PerformanceAnalyzer::analyzeQueries(Smokin::PrinterDataAnalyzer());
	Smokin::PerformanceAnalyzer::analyzeQueries(Smokin::QueryCounterPrinterDataAnalyzer());
	Smokin::PerformanceAnalyzer::analyzeQueries(Smokin::StatsPrinterDataAnalyzer());

	cin.get();
}