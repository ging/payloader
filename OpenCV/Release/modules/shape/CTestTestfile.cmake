# CMake generated Testfile for 
# Source directory: /home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/modules/shape
# Build directory: /home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/modules/shape
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(opencv_test_shape "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/bin/opencv_test_shape" "--gtest_output=xml:opencv_test_shape.xml")
SET_TESTS_PROPERTIES(opencv_test_shape PROPERTIES  LABELS "Main;opencv_shape;Accuracy" WORKING_DIRECTORY "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/test-reports/accuracy")
