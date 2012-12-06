for folder in $(ls -d1 */)
do
	echo $folder
	cd $folder
	len=${#folder}-1
	project_name=${folder:0:len}
	echo $project_name

	xcodebuild -configuration Release -target "$project_name" -project "$project_name.xcodeproj"   
	cd ../
	if [ "$?" != "0" ]; then
		echo failed building $project_name
	else
		echo "example " $project_name " builted"
	fi
done
