// kate: replace-tabs off; indent-width 4; indent-mode normal
// vim: ts=4:sw=4:noexpandtab
/*
Copyright (c) 2010--2012,
François Pomerleau and Stephane Magnenat, ASL, ETHZ, Switzerland
You can contact the authors at <f dot pomerleau at gmail dot com> and
<stephane at magnenat dot net>
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ETH-ASL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "pointmatcher/PointMatcher.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include "boost/filesystem.hpp"
#include <ctime>

using namespace std;

using PM = PointMatcher<float>;

void validateArgs(int argc, char *argv[], bool& isCSV);

/**
  * Code example for ICP taking 2 points clouds (2D or 3D) relatively close
  * and computing the transformation between them.
  */
int main(int argc, char *argv[])
{
    bool isCSV = true;
    validateArgs(argc, argv, isCSV);

    typedef PM::DataPoints DP;

    // Load point clouds
    const DP ref(DP::load(argv[1]));
    const DP data(DP::load(argv[2]));

    // print labels
    cout << "ref labels: ";
    for(const auto p: ref.featureLabels){
        cout << p.text << " " ;
    }
    cout << endl;
    cout << "ref col 0: ";
    for(int row =0; row < 4; row++){
        cout << ref.features(row, 0) << " ";
    }
    cout <<endl;

    // Create the default ICP algorithm
    PM::ICP icp;

    // See the implementation of setDefault() to create a custom ICP algorithm
    icp.setDefault();

    cout << "data size: " << data.features.rows() << " * " << data.features.cols() << endl;
    // Transform data to express it in ref
    DP data_ref(ref);
    DP data_out(data);

    const clock_t begin_time = clock();

    // Compute the transformation to express data in ref
    PM::TransformationParameters T = icp(data, data_ref);
    icp.transformations.apply(data_out, T);

    std::cout << "Transform time cost: " << float( clock () - begin_time )*1000 /  CLOCKS_PER_SEC << " ms" << std::endl;



    // Safe files to see the results
    //ref.save("test_ref.vtk");
    //data.save("test_data_in.vtk");
//    data_out.save("test_data_out.vtk");
    cout << "Final transformation:" << endl << T << endl;

    return 0;
}

void validateArgs(int argc, char *argv[], bool& isCSV )
{
    if (argc < 3)
    {
        cerr << "Wrong number of arguments, usage " << argv[0] << " reference.csv reading.csv" << endl;
        cerr << "Will create 3 vtk files for inspection: ./test_ref.vtk, ./test_data_in.vtk and ./test_data_out.vtk" << endl;
        cerr << endl << "2D Example:" << endl;
        cerr << "  " << argv[0] << " ../../examples/data/2D_twoBoxes.csv ../../examples/data/2D_oneBox.csv ../data/default-convert.yaml" << endl;
        cerr << endl << "3D Example:" << endl;
        cerr << "  " << argv[0] << " ../../examples/data/car_cloud400.csv ../../examples/data/car_cloud401.csv" << endl;
        exit(1);
    }
}
