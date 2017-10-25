# CMake generated Testfile for 
# Source directory: /home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/modules/video
# Build directory: /home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/modules/video
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(opencv_test_video "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/bin/opencv_test_video" "--gtest_output=xml:opencv_test_video.xml")
SET_TESTS_PROPERTIES(opencv_test_video PROPERTIES  LABELS "Main;opencv_video;Accuracy" WORKING_DIRECTORY "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/test-reports/accuracy")
ADD_TEST(opencv_perf_video "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/bin/opencv_perf_video" "--gtest_output=xml:opencv_perf_video.xml")
SET_TESTS_PROPERTIES(opencv_perf_video PROPERTIES  LABELS "Main;opencv_video;Performance" WORKING_DIRECTORY "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/test-reports/performance")
ADD_TEST(opencv_sanity_video "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/bin/opencv_perf_video" "--gtest_output=xml:opencv_perf_video.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
SET_TESTS_PROPERTIES(opencv_sanity_video PROPERTIES  LABELS "Main;opencv_video;Sanity" WORKING_DIRECTORY "/home/nacho/Documentos/Proyectos/Arqueopterix/pay_good/payloader/OpenCV/Release/test-reports/sanity")
