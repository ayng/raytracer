task :default do
  if !Dir.exists?("build")
    sh "mkdir build"
  end
  Dir.chdir("build") do
    sh "cmake .."
    sh "make"
  end
end

task :clean do
  sh "rm -rf build"
end

task :run do
  output = `build/raytracer -o cube.png -r 400 -aa 3 < scenes/cube.txt`
end
task :test do
  sh "build/rt_test"
end
