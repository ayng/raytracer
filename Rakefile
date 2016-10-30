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

cmds = {'cube.png' => 'build/raytracer -o cube.png -r 400 -aa 3 < scenes/cube.txt',
        'refl.png' => 'build/raytracer -o refl.png -r 400 -aa 3 < scenes/journal_reflection.txt'}

task :run do
  cmds.each do |filename, cmd|
    puts filename
    sh cmd
  end
end
task :test do
  sh "build/rt_test"
end
