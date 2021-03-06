#pragma once
#ifndef TKP_MAINWINDOW_HXX
#define TKP_MAINWINDOW_HXX
#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QLabel>
#include <memory>
#include <array>
#include "../include/emulator_factory.h"
#include "../include/emulator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // Initialization functions
    void create_actions();
    void create_menus();

    // Menu bar actions
    void open_file();
    void open_settings();
    void open_about();
    void open_debugger();
    void open_tracelogger();
    void screenshot();
    void close_tools();

    // Emulation functions
    void pause_emulator();
    void reset_emulator();
    void stop_emulator();
    void enable_emulation_actions(bool should);
    void setup_emulator_specific();

private slots:
    void redraw_screen();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMenu* file_menu_;
    QMenu* emulation_menu_;
    QMenu* tools_menu_;
    QMenu* help_menu_;
    QAction* open_act_;
    QAction* pause_act_;
    QAction* reset_act_;
    QAction* about_act_;
    QAction* stop_act_;
    QAction* settings_act_;
    QAction* screenshot_act_;
    QAction* debugger_act_;
    QAction* tracelogger_act_;
    QLabel* lbl_;
    QPixmap texture_;
    std::shared_ptr<TKPEmu::Tools::MQBase> message_queue_;
    std::shared_ptr<TKPEmu::Emulator> emulator_;
    std::array<QWidget*, 2> emulator_tools_ {};
    TKPEmu::EmuType emulator_type_;
    std::thread emulator_thread_;
    bool settings_open_ = false;
    bool about_open_ = false;
    bool debugger_open_ = false;
    bool tracelogger_open_ = false;
};
#endif // MAINWINDOW_HXX
