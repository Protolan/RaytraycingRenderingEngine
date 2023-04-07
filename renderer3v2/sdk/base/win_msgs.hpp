/// @file
///
/// @brief Dealing with threads, declarations.
///
/// The described here functions provide a unified interface for Multithread
/// supporting. The current implementation is intended
/// only for the Windows platform.
///
/// @sa @ref base_mainpage
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Pbb - Boris Barladian, '00.12.07
/// @author Pls - Lev Shapiro, '00.12.07

#ifndef KCVR_WIN_MSGS_HPP_
#define KCVR_WIN_MSGS_HPP_

INTEGRA_NAMESPACE_START



/// @name Message identifiers for Windows only
//@{
/// @note Identifiers are added to Windows WM_User in IntSendMessage() procedure.
enum IDM_Messages
  {
  /// Identifier for OTF dialog creation.
  IDM_OTFDLG_CREATE = 1000,
  /// Identifier for OTF dialog acception.
  IDM_OTFDLG_OK,        //  1001
  /// Identifier for OTF dialog rejection.
  IDM_OTFDLG_CANCEL,    //  1002
  /// Identifier for Lumicept LumiVue and I2Server exit message.
  IDM_EXIT = 2000,
  /// Identifier for I2Server load scene message.
  IDM_VR_SCENE_LOAD,
  /// Identifier for I2Server load scene message.
  IDM_VR_SCENE_UNLOAD,
  /// Identifier for I2Server render by Path Tracer message.
  IDM_RENDER_PT,
  /// Identifier for I2Server render by Path Tracer message (first pass of multi-pass mode).
  IDM_RENDER_PT_MULT_FIRST,
  /// Identifier for I2Server render by Path Tracer message (following passes of multi-pass mode).
  IDM_RENDER_PT_MULT_NEXT,
  /// Identifier for I2Server VR calculation message.
  IDM_VR_CALCULATE,
  /// Identifier for I2Server VR analysis message.
  IDM_VR_ANALYZE,
  /// Identifier for I2Server VR ray history message.
  IDM_VR_RAY_HISTORY,
  /// Identifier for I2Server VR criteria message.
  IDM_VR_RAY_CRITERIA,
  /// Identifier for I2Server VR coloring conditions dialog.
  IDM_VR_RAY_COL_CONDITIONS,
  /// Identifier for I2Server face node index calculation.
  IDM_VR_NODE_INDEX_CALC,
  /// Identifier for I2Server observer result file name calculation.
  IDM_VR_OBS_RES_FILE_NAME,
  /// Load file by LumiVue.
  IDM_LUMIVUE_LOAD,
  /// Hide LumiVue.
  IDM_LUMIVUE_HIDE,
  /// Get LumiVue detector area parameters.
  IDM_LUMIVUE_DETECTOR,
  /// Identifier for medium editing.
  IDM_MEDIUM_EDIT,
  /// Identifier for substance editing.
  IDM_SUBSTANCE_EDIT,
  /// Put medium to library.
  IDM_MEDIUM_PUT_TO_LIB,
  /// Get medium from library.
  IDM_MEDIUM_GET_FROM_LIB,
  /// Put substance to library.
  IDM_SUBSTANCE_PUT_TO_LIB,
  /// Get substance from library.
  IDM_SUBSTANCE_GET_FROM_LIB,
  /// Initialize servers.
  IDM_INIT_SERVERS,
  /// Servers control (invoke servers control dialog).
  IDM_SERVERS_CONTROL,
  /// Calculate the CATIA mask.
  IDM_CALC_CATIA_MASK,
  /// Identifier for OPTOS editing.
  IDM_OPTOS_EDIT,
  /// Identifier for OPTOS editing close.
  IDM_OPTOS_EDIT_CLOSE,
  /// Identifier for HDRI panorama editing.
  IDM_HDRI_EDIT,
  /// Identifier for daylight editing.
  IDM_DAYLIGHT_EDIT,
  /// Identifier for INI loading command.
  IDM_INI_LOAD,
  /// Identifier for UNI loading command (units setup file.
  IDM_UNI_LOAD,
  /// Maximal possible message (should always be at the end of the list).
  IDM_MAX_MESSAGE
  };
//@}

INTEGRA_NAMESPACE_END

#endif  // KCVR_WIN_MSGS_HPP_

