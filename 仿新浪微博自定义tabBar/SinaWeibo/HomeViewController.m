//
//  HomeViewController.m
//  SinaWeibo
//
//  Created by lanou on 16/6/4.
//  Copyright © 2016年 xunjiangtao. All rights reserved.
//

#import "HomeViewController.h"
#import "TitleButton.h"


@implementation HomeViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setTitleButton];
    
}

// 设置中间标题按钮
- (void)setTitleButton {
    TitleButton *button = [[TitleButton alloc] init];
    [button setTitle:@"首页" forState:UIControlStateNormal];
    [button addTarget:self action:@selector(titleClick:) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.titleView = button;
}

- (void)titleClick:(UIButton *)button {
    
}

@end
