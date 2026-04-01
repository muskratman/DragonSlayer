// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * FCookieBrosLevelEditorModule
 * Editor-only module that registers the 2.5D Tile Editor mode, detail customizations,
 * and supporting Slate UI (palette widget, commands).
 */
class FCookieBrosLevelEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
