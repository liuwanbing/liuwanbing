//
//  GGRecomHeaderView.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGRecomHeaderView.h"

@implementation GGRecomHeaderView

- (void)awakeFromNib {
    // Initialization code
}
-(void)setRecomHeaderModel:(GGRecomHeaderModel *)recomHeaderModel{
    self.name.text = recomHeaderModel.name;
    
}
@end
