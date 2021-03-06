<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{<!PROJECT_GUID!>}</ProjectGuid>
    <RootNamespace><!PROJECT_NAME!></RootNamespace>
    <Keyword>Win32Proj</Keyword>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>MultiByte</CharacterSet>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseOfMfc>false</UseOfMfc>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>MultiByte</CharacterSet>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseOfMfc>false</UseOfMfc>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="PropertySheets">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC71.props" />
  </ImportGroup>
  <ImportGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="PropertySheets">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC71.props" />
  </ImportGroup>
  <ImportGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="PropertySheets">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC71.props" />
  </ImportGroup>
  <ImportGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="PropertySheets">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC71.props" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup>
    <_ProjectFileVersion>10.0.30319.1</_ProjectFileVersion>
    <OutDir Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">.\</OutDir>
    <IntDir Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'"><!LSDK_PATH!>\Out\<!PROJECT_NAME!>Debug32\</IntDir>
    <LinkIncremental Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">true</LinkIncremental>
    <OutDir Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">.\</OutDir>
    <IntDir Condition="'$(Configuration)|$(Platform)'=='Release|Win32'"><!LSDK_PATH!>\Out\<!PROJECT_NAME!>Release32\</IntDir>
    <LinkIncremental Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">false</LinkIncremental>
    <OutDir Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">.\</OutDir>
    <IntDir Condition="'$(Configuration)|$(Platform)'=='Debug|x64'"><!LSDK_PATH!>\Out\<!PROJECT_NAME!>Debug64\</IntDir>
    <LinkIncremental Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">true</LinkIncremental>
    <OutDir Condition="'$(Configuration)|$(Platform)'=='Release|x64'">.\</OutDir>
    <IntDir Condition="'$(Configuration)|$(Platform)'=='Release|x64'"><!LSDK_PATH!>\Out\<!PROJECT_NAME!>Release64\</IntDir>
    <LinkIncremental Condition="'$(Configuration)|$(Platform)'=='Release|x64'">false</LinkIncremental>
    <CodeAnalysisRuleSet Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">AllRules.ruleset</CodeAnalysisRuleSet>
    <CodeAnalysisRules Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" />
    <CodeAnalysisRuleAssemblies Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" />
    <CodeAnalysisRuleSet Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">AllRules.ruleset</CodeAnalysisRuleSet>
    <CodeAnalysisRules Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" />
    <CodeAnalysisRuleAssemblies Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" />
    <CodeAnalysisRuleSet Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">AllRules.ruleset</CodeAnalysisRuleSet>
    <CodeAnalysisRules Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" />
    <CodeAnalysisRuleAssemblies Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" />
    <CodeAnalysisRuleSet Condition="'$(Configuration)|$(Platform)'=='Release|x64'">AllRules.ruleset</CodeAnalysisRuleSet>
    <CodeAnalysisRules Condition="'$(Configuration)|$(Platform)'=='Release|x64'" />
    <CodeAnalysisRuleAssemblies Condition="'$(Configuration)|$(Platform)'=='Release|x64'" />
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LocalDebuggerCommand>$(OutDir)<!PROJECT_NAME!>_Win32Debug.exe</LocalDebuggerCommand>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    <TargetName><!PROJECT_NAME!>_Win32Debug</TargetName>
    <TargetExt>.exe</TargetExt>
    <TargetPath>$(OutDir)<!PROJECT_NAME!>_Win32Debug.exe</TargetPath>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <LocalDebuggerCommand>$(OutDir)<!PROJECT_NAME!>_Win64Debug.exe</LocalDebuggerCommand>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    <TargetName><!PROJECT_NAME!>_Win64Debug</TargetName>
    <TargetExt>.exe</TargetExt>
    <TargetPath>$(OutDir)<!PROJECT_NAME!>_Win64Debug.exe</TargetPath>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <LocalDebuggerCommand>$(OutDir)<!PROJECT_NAME!>_Win32Release.exe</LocalDebuggerCommand>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    <TargetName><!PROJECT_NAME!>_Win32Release</TargetName>
    <TargetExt>.exe</TargetExt>
    <TargetPath>$(OutDir)<!PROJECT_NAME!>_Win32Release.exe</TargetPath>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <LocalDebuggerCommand>$(OutDir)<!PROJECT_NAME!>_Win64Release.exe</LocalDebuggerCommand>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    <TargetName><!PROJECT_NAME!>_Win64Release</TargetName>
    <TargetExt>.exe</TargetExt>
    <TargetPath>$(OutDir)<!PROJECT_NAME!>_Win64Release.exe</TargetPath>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <AdditionalOptions>/MP /J %(AdditionalOptions)</AdditionalOptions>
      <Optimization>Disabled</Optimization>
      <IntrinsicFunctions>false</IntrinsicFunctions>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <OmitFramePointers>true</OmitFramePointers>
      <AdditionalIncludeDirectories><!LSDK_PATH!>\Src;.\Src\;<!LSDK_PATH!>\Src\Linderdaum;.\Src;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <StringPooling>false</StringPooling>
      <MinimalRebuild>false</MinimalRebuild>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <SmallerTypeCheck>true</SmallerTypeCheck>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <StructMemberAlignment>8Bytes</StructMemberAlignment>
      <EnableEnhancedInstructionSet>StreamingSIMDExtensions2</EnableEnhancedInstructionSet>
      <RuntimeTypeInfo>true</RuntimeTypeInfo>
      <PrecompiledHeader>
      </PrecompiledHeader>
      <AssemblerOutput>
      </AssemblerOutput>
      <BrowseInformation>false</BrowseInformation>
      <WarningLevel>Level4</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
    </ClCompile>
    <Link>
      <AdditionalOptions>/fixed:no %(AdditionalOptions)</AdditionalOptions>
      <AdditionalDependencies>LinderdaumEngine.lib;user32.lib;gdi32.lib;Vfw32.lib;ole32.lib;fmodex_vc.lib;%(AdditionalDependencies)</AdditionalDependencies>
      <OutputFile>$(OutDir)<!PROJECT_NAME!>_Win32Debug.exe</OutputFile>
      <AdditionalLibraryDirectories><!LSDK_PATH!>\Libs.Win32;<!LSDK_PATH!>\Out\Debug32;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <ProgramDatabaseFile>$(OutDir)<!PROJECT_NAME!>.pdb</ProgramDatabaseFile>
      <SubSystem>Console</SubSystem>
      <TargetMachine>MachineX86</TargetMachine>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <AdditionalOptions>/MP /J %(AdditionalOptions)</AdditionalOptions>
      <Optimization>Full</Optimization>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <OmitFramePointers>true</OmitFramePointers>
      <AdditionalIncludeDirectories><!LSDK_PATH!>\Src;.\Src\;<!LSDK_PATH!>\Src\Linderdaum;.\Src;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
      <StructMemberAlignment>8Bytes</StructMemberAlignment>
      <BufferSecurityCheck>false</BufferSecurityCheck>
      <EnableEnhancedInstructionSet>StreamingSIMDExtensions2</EnableEnhancedInstructionSet>
      <RuntimeTypeInfo>true</RuntimeTypeInfo>
      <PrecompiledHeader>
      </PrecompiledHeader>
      <AssemblerOutput>
      </AssemblerOutput>
      <WarningLevel>Level4</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
    </ClCompile>
    <Link>
      <AdditionalDependencies>LinderdaumEngine.lib;user32.lib;gdi32.lib;Vfw32.lib;ole32.lib;fmodex_vc.lib;%(AdditionalDependencies)</AdditionalDependencies>
      <OutputFile>$(OutDir)<!PROJECT_NAME!>_Win32Release.exe</OutputFile>
      <AdditionalLibraryDirectories><!LSDK_PATH!>\Libs.Win32;<!LSDK_PATH!>\Out\Release32;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <SubSystem>Console</SubSystem>
      <OptimizeReferences>true</OptimizeReferences>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <TargetMachine>MachineX86</TargetMachine>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Midl>
      <TargetEnvironment>X64</TargetEnvironment>
    </Midl>
    <ClCompile>
      <AdditionalOptions>/MP /J %(AdditionalOptions)</AdditionalOptions>
      <Optimization>Disabled</Optimization>
      <IntrinsicFunctions>false</IntrinsicFunctions>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <OmitFramePointers>true</OmitFramePointers>
      <AdditionalIncludeDirectories><!LSDK_PATH!>\Src;.\Src\;<!LSDK_PATH!>\Src\Linderdaum;.\Src;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <StringPooling>false</StringPooling>
      <MinimalRebuild>false</MinimalRebuild>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <SmallerTypeCheck>true</SmallerTypeCheck>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <StructMemberAlignment>8Bytes</StructMemberAlignment>
      <EnableEnhancedInstructionSet>NotSet</EnableEnhancedInstructionSet>
      <RuntimeTypeInfo>true</RuntimeTypeInfo>
      <PrecompiledHeader>
      </PrecompiledHeader>
      <AssemblerOutput>
      </AssemblerOutput>
      <BrowseInformation>false</BrowseInformation>
      <WarningLevel>Level4</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
    </ClCompile>
    <Link>
      <AdditionalOptions>/fixed:no %(AdditionalOptions)</AdditionalOptions>
      <AdditionalDependencies>LinderdaumEngine.lib;user32.lib;gdi32.lib;Vfw32.lib;ole32.lib;fmodex64_vc.lib;%(AdditionalDependencies)</AdditionalDependencies>
      <OutputFile>$(OutDir)<!PROJECT_NAME!>_Win64Debug.exe</OutputFile>
      <AdditionalLibraryDirectories><!LSDK_PATH!>\Libs.Win64;<!LSDK_PATH!>\Out\Debug64;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <ProgramDatabaseFile>$(OutDir)<!PROJECT_NAME!>.pdb</ProgramDatabaseFile>
      <SubSystem>Console</SubSystem>
      <TargetMachine>MachineX64</TargetMachine>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Midl>
      <TargetEnvironment>X64</TargetEnvironment>
    </Midl>
    <ClCompile>
      <AdditionalOptions>/MP /J %(AdditionalOptions)</AdditionalOptions>
      <Optimization>Full</Optimization>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <OmitFramePointers>true</OmitFramePointers>
      <AdditionalIncludeDirectories><!LSDK_PATH!>\Src;.\Src\;<!LSDK_PATH!>\Src\Linderdaum;.\Src;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
      <StructMemberAlignment>8Bytes</StructMemberAlignment>
      <BufferSecurityCheck>false</BufferSecurityCheck>
      <EnableEnhancedInstructionSet>NotSet</EnableEnhancedInstructionSet>
      <RuntimeTypeInfo>true</RuntimeTypeInfo>
      <PrecompiledHeader>
      </PrecompiledHeader>
      <AssemblerOutput>
      </AssemblerOutput>
      <WarningLevel>Level4</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <InlineFunctionExpansion>OnlyExplicitInline</InlineFunctionExpansion>
      <FloatingPointModel>Fast</FloatingPointModel>
    </ClCompile>
    <Link>
      <AdditionalDependencies>LinderdaumEngine.lib;user32.lib;gdi32.lib;Vfw32.lib;ole32.lib;fmodex64_vc.lib;%(AdditionalDependencies)</AdditionalDependencies>
      <OutputFile>$(OutDir)<!PROJECT_NAME!>_Win64Release.exe</OutputFile>
      <AdditionalLibraryDirectories><!LSDK_PATH!>\Libs.Win64;<!LSDK_PATH!>\Out\Release64;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <SubSystem>Console</SubSystem>
      <OptimizeReferences>true</OptimizeReferences>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <TargetMachine>MachineX64</TargetMachine>
    </Link>
  </ItemDefinitionGroup>