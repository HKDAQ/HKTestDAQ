#include "Loader.h"

Loader::Loader():Tool(){}


bool Loader::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_variables.Get("in", infile);
	m_variables.Get("out", outfile);
	
	std::ifstream fin(infile.c_str());
	while (getline(fin, emp))
		m_data->vData.push_back(emp);
	m_data->vRes.resize(m_data->vData.size());
	std::cout << "Data loaded " << m_data->vData.size() << std::endl;

	return true;
}


bool Loader::Execute(){

	return true;
}


bool Loader::Finalise(){

	std::ofstream fout(outfile.c_str());
	std::cout << "RES SIZE   " << m_data->vRes.size() << std::endl;
	for (int i = 0; i < m_data->vData.size(); i++)
		if (m_data->vRes.at(i)) fout << i << std::endl;

	return true;
}
