# CMake generated Testfile for 
# Source directory: /home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/modules/ml
# Build directory: /home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/build/modules/ml
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(opencv_test_ml "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/build/bin/opencv_test_ml" "--gtest_output=xml:opencv_test_ml.xml")
SET_TESTS_PROPERTIES(opencv_test_ml PROPERTIES  LABELS "Main;opencv_ml;Accuracy" WORKING_DIRECTORY "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/build/test-reports/accuracy")
