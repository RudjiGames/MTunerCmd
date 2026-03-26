--
-- Copyright 2025 Milos Tosic. All rights reserved.   
-- License: http://www.opensource.org/licenses/BSD-2-Clause
--

function projectDependencies_MTunerCmd()
	return { "rbase", "rdebug" }
end

function projectAdd_MTunerCmd()
	addProject_cmd("MTunerCmd")
end
