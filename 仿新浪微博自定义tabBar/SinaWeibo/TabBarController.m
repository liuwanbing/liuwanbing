//
//  TabBarController.m
//  SinaWeibo
//
//  Created by lanou on 16/6/4.
//  Copyright © 2016年 xunjiangtao. All rights reserved.
//

#import "TabBarController.h"
#import "JTNavigationController.h"
#import "HomeViewController.h"
#import "MessageViewController.h"
#import "DiscoverViewController.h"
#import "ProfileViewController.h"
#import "UIView+Expand.h"
// RGB颜色
#define RGBColor(r, g, b) [UIColor colorWithRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:1.0]

// 随机色
#define RandomColor RGBColor(arc4random_uniform(256), arc4random_uniform(256), arc4random_uniform(256))

#define kWidth [UIScreen mainScreen].bounds.size.width
#define kHeight [UIScreen mainScreen].bounds.size.height

@interface TabBarController()

@property(nonatomic, strong) UIVisualEffectView *effectView;

@property(nonatomic, strong) UIImageView *imageView;

@end


@implementation TabBarController

// 添加子控制器
- (void)addChildViewController:(UIViewController *)childController title:(NSString *)title image:(NSString *)image selectedImage:(NSString *)selectedImage {
    childController.tabBarItem.title = title;
    childController.tabBarItem.image = [UIImage imageNamed:image];
    
    // 禁用图片渲染，保持图片原样
    childController.tabBarItem.selectedImage = [[UIImage imageNamed:selectedImage] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal];
    
    // 设置文字的样式
    [childController.tabBarItem setTitleTextAttributes:@{NSForegroundColorAttributeName:RGBColor(123, 123, 123)} forState:UIControlStateNormal];
    [childController.tabBarItem setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor orangeColor]} forState:UIControlStateSelected];
    
    JTNavigationController *naviController = [[JTNavigationController alloc] initWithRootViewController:childController];
    [self addChildViewController:naviController];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self addChildViewController:[[HomeViewController alloc] init] title:@"首页" image:@"tabbar_home" selectedImage:@"tabbar_home_selected"];
    [self addChildViewController:[[MessageViewController alloc] init] title:@"消息" image:@"tabbar_message_center" selectedImage:@"tabbar_message_center_selected"];
    [self addChildViewController:[[DiscoverViewController alloc] init] title:@"发现" image:@"tabbar_discover" selectedImage:@"tabbar_discover_selected"];
    [self addChildViewController:[[ProfileViewController alloc] init] title:@"我" image:@"tabbar_profile" selectedImage:@"tabbar_profile_selected"];
    
    CustomTabBar *tabBar = [[CustomTabBar alloc] init];
    tabBar.delegate = self;
    
    // kvc:如果要修改系统某些readOnly的属性，就使用KVC
    [self setValue:tabBar forKey:@"tabBar"];
}

// 实现代理方法
- (void)tabBarDidClickCenterButton:(CustomTabBar *)tabBar {
    
    UIGraphicsBeginImageContextWithOptions([UIScreen mainScreen].bounds.size, NO, 0);
    [self.selectedViewController.view.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    self.imageView = [[UIImageView alloc] initWithImage:image];
    self.imageView.bounds = [UIScreen mainScreen].bounds;
    [self.view addSubview:self.imageView];
    
    UIBlurEffect *blur = [UIBlurEffect effectWithStyle:UIBlurEffectStyleDark];
    self.effectView = [[UIVisualEffectView alloc] initWithEffect:blur];
    self.effectView.frame = self.imageView.frame;
    [self.imageView addSubview:self.effectView];
    self.imageView.userInteractionEnabled = YES;
   
    // 创建按钮
    UIButton *chatBtn = [self AddButtonOnView:@"button-chat" lineIndex:0 rowIndex:0];
    UIButton *linkBtn = [self AddButtonOnView:@"button-link" lineIndex:1 rowIndex:0];
    UIButton *photoBtn = [self AddButtonOnView:@"button-photo" lineIndex:2 rowIndex:0];
    UIButton *quoteBtn = [self AddButtonOnView:@"button-quote" lineIndex:0 rowIndex:1];
    UIButton *textBtn = [self AddButtonOnView:@"button-text" lineIndex:1 rowIndex:1];
    UIButton *videoBtn = [self AddButtonOnView:@"button-video" lineIndex:2 rowIndex:1];
    
    UIButton *cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [cancelBtn setBackgroundImage:[UIImage imageNamed:@"button-cancel"] forState:UIControlStateNormal];
    cancelBtn.center = CGPointMake(kWidth / 2 - 18, kHeight - 60);
    cancelBtn.size = cancelBtn.currentBackgroundImage.size;
    [cancelBtn addTarget:self action:@selector(back:) forControlEvents:UIControlEventTouchUpInside];
    
    [self.effectView addSubview:cancelBtn];
    
    
}

// 封装添加按钮的方法
- (UIButton *)AddButtonOnView:(NSString *)backgroundImage lineIndex:(NSUInteger)lineIndex rowIndex:(NSUInteger)rowIndex {
    
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [button setBackgroundImage:[UIImage imageNamed:backgroundImage] forState:UIControlStateNormal];
    
    CGFloat btnWidth = (self.effectView.frame.size.width - 80) / 6;
    CGFloat btnHeight = button.currentBackgroundImage.size.height;
    button.frame = CGRectMake(80 + btnWidth * lineIndex, kHeight + (btnHeight + 20) * rowIndex, button.currentBackgroundImage.size.width, btnHeight);
    
    [self.effectView addSubview:button];
    
    [UIView animateWithDuration:0.3 animations:^{
        button.center = CGPointMake(40 + btnWidth *(lineIndex * 2 + 1), 250 + (btnHeight + 20) * rowIndex);
        button.size = button.currentBackgroundImage.size;
    }];
    
    return button;
}

// 退出按钮的事件
- (void)back:(UIButton *)button {
    [self.effectView removeFromSuperview];
    [self.imageView removeFromSuperview];
}


@end
